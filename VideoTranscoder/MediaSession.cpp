#include "stdafx.h"
#include "MediaSession.hpp"

#include <iostream>
#include <propvarutil.h>
#include <MinCppXtra/win32_errors.hpp>

#include "AppException.hpp"

namespace application
{
    static HRESULT CreateEventHandle(HANDLE* eventHandle)
    {
        *eventHandle = CreateEventW(nullptr, FALSE, FALSE, nullptr);
        if (*eventHandle == nullptr)
        {
            return HRESULT_FROM_WIN32(GetLastError());
        }
        return S_OK;
    }

    MediaSession::MediaSession()
        : m_refCount(0)
        , m_hrStatus(S_OK)
        , m_closedSessionEventHandle(nullptr)
    {
        CHECK("create media session",
            MFCreateMediaSession(nullptr, m_mfMediaSession.GetAddressOf()));

        ComPtr<IMFClock> mfClock;
        CHECK("get media session clock", m_mfMediaSession->GetClock(mfClock.GetAddressOf()));
        CHECK("get presentation clock",
            mfClock->QueryInterface(IID_PPV_ARGS(m_presentationClock.GetAddressOf())));

        CHECK("begin async callback to get media event",
            m_mfMediaSession->BeginGetEvent(this, nullptr));

        CHECK("create event handle", CreateEventHandle(&m_closedSessionEventHandle));
    }

    MediaSession::~MediaSession()
    {
        LOG("shutdown media session", m_mfMediaSession->Shutdown());
        CloseHandle(m_closedSessionEventHandle);
    }

    STDMETHODIMP MediaSession::GetParameters(DWORD* pdwFlags, DWORD* pdwQueue)
    {
        return E_NOTIMPL;
    }

    STDMETHODIMP MediaSession::QueryInterface(REFIID riid, void** ppv)
    {
        static const QITAB qit[] =
        {
            QITABENT(MediaSession, IMFAsyncCallback),
            { 0 }
        };
        return QISearch(this, qit, riid, ppv);
    }

    STDMETHODIMP_(ULONG) MediaSession::AddRef()
    {
        return InterlockedIncrement(&m_refCount);
    }

    STDMETHODIMP_(ULONG) MediaSession::Release()
    {
        long refCount = InterlockedDecrement(&m_refCount);
        if (refCount == 0)
        {
            delete this;
        }
        return refCount;
    }

    STDMETHODIMP MediaSession::Invoke(IMFAsyncResult* result)
    {
        try
        {
            ComPtr<IMFMediaEvent> mfMediaEvent;
            CHECK("end async callback to get media event",
                m_mfMediaSession->EndGetEvent(result, mfMediaEvent.GetAddressOf()));

            MediaEventType meType = MEUnknown;
            CHECK("get media event type", mfMediaEvent->GetType(&meType));

            HRESULT eventStatus = S_OK;
            CHECK("get media event status", mfMediaEvent->GetStatus(&eventStatus));

            if (FAILED(eventStatus))
            {
                LOG("close media session", m_mfMediaSession->Close());
                return eventStatus;
            }

            switch (meType)
            {
            case MESessionEnded:
                LOG("close media session", m_mfMediaSession->Close());
                break;

            case MESessionClosed:
                SetEvent(m_closedSessionEventHandle);
                break;
            }

            if (meType != MESessionClosed)
            {
                CHECK("begin async callback to get media event",
                    m_mfMediaSession->BeginGetEvent(this, nullptr));
            }
        }
        catch (AppException& ex)
        {
            std::cerr << std::endl << ex.Serialize() << std::endl;
            LOG("close media session", m_mfMediaSession->Close());
            return m_hrStatus = ex.GetHResult().value_or(E_UNEXPECTED);
        }

        return S_OK;
    }

    void MediaSession::StartEncodingSession(const ComPtr<IMFTopology>& topology)
    {
        CHECK("set topology in media session",
            m_mfMediaSession->SetTopology(0, topology.Get()));

        PROPVARIANT varStart;
        PropVariantInit(&varStart);
        CHECK("start media session", m_mfMediaSession->Start(&GUID_NULL, &varStart));
    }

    std::chrono::nanoseconds MediaSession::GetEncodingPosition() const
    {
        MFTIME mfTime;
        CHECK("get presentation time", m_presentationClock->GetTime(&mfTime));
        return std::chrono::nanoseconds(mfTime * 100);
    }

    HRESULT MediaSession::Wait(std::chrono::milliseconds timeout) const
    {
        DWORD waitResult = WaitForSingleObject(
            m_closedSessionEventHandle, static_cast<DWORD>(timeout.count()));

        switch (waitResult)
        {
        case WAIT_ABANDONED:
        case WAIT_TIMEOUT:
            return E_PENDING;

        case WAIT_OBJECT_0:
            return m_hrStatus;

        case WAIT_FAILED:
            throw AppException(
                mincpp::Win32Errors::GetErrorMessage(
                    GetLastError(), "WaitForSingleObject"));

        default:
            return E_FAIL;
        }
    }
}
