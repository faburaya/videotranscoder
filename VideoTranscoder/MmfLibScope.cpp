#include "stdafx.h"
#include "MmfLibScope.hpp"

#include "AppException.hpp"

namespace application
{
    /// <summary>
    /// Initializes a new instance of the <see cref="MmfLibScope"/> class.
    /// </summary>
    MmfLibScope::MmfLibScope()
    {
        CHECK("initialize COM library", CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED));
        CHECK("start MF library", MFStartup(MF_VERSION, MFSTARTUP_LITE));
    }

    /// <summary>
    /// Finalizes an instance of the <see cref="MmfLibScope"/> class.
    /// </summary>
    MmfLibScope::~MmfLibScope()
    {
        LOG("shutdown MF library", MFShutdown());
        CoUninitialize();
    }
}