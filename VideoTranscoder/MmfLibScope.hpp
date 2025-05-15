#pragma once

namespace application
{
    /// <summary>
    /// Uses RAII to initialize and finalize Microsoft Media Foundation library.
    /// </summary>
    class MmfLibScope
    {
    public:

        MmfLibScope();
        ~MmfLibScope();
    };
}
