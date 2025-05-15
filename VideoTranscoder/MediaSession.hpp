#pragma once

#include <chrono>

#include <Windows.h>
#include <mfobjects.h>
#include <wrl.h>

namespace application
{
    using namespace Microsoft::WRL;

    class MediaSession : public IMFAsyncCallback
    {
    private:

        ComPtr<IMFMediaSession> m_mfMediaSession;
        ComPtr<IMFPresentationClock> m_presentationClock;
        HRESULT m_hrStatus;
        HANDLE  m_closedSessionEventHandle;
        long    m_refCount;

    public:

        MediaSession();
        virtual ~MediaSession();

        // IUnknown methods
        STDMETHODIMP QueryInterface(REFIID riid, void** ppv);
        STDMETHODIMP_(ULONG) AddRef();
        STDMETHODIMP_(ULONG) Release();

        // IMFAsyncCallback methods
        STDMETHODIMP GetParameters(DWORD* pdwFlags, DWORD* pdwQueue);
        STDMETHODIMP Invoke(IMFAsyncResult* result);

        void StartEncodingSession(const ComPtr<IMFTopology>& topology);

        std::chrono::nanoseconds GetEncodingPosition() const;

        HRESULT Wait(std::chrono::milliseconds timeout) const;
    };
}
