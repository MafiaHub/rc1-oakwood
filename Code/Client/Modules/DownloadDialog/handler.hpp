namespace dldialog
{
    class CallbackHandler : public IBindStatusCallback
    {
    private:
        int m_percentLast;

    public:
        CallbackHandler() : m_percentLast(0)
        {
        }

        // IUnknown

        HRESULT STDMETHODCALLTYPE
            QueryInterface(REFIID riid, void** ppvObject)
        {

            if (IsEqualIID(IID_IBindStatusCallback, riid)
                || IsEqualIID(IID_IUnknown, riid))
            {
                *ppvObject = reinterpret_cast<void*>(this);
                return S_OK;
            }

            return E_NOINTERFACE;
        }

        ULONG STDMETHODCALLTYPE
            AddRef()
        {
            return 2UL;
        }

        ULONG STDMETHODCALLTYPE
            Release()
        {
            return 1UL;
        }

        // IBindStatusCallback

        HRESULT STDMETHODCALLTYPE
            OnStartBinding(DWORD     /*dwReserved*/,
                IBinding* /*pib*/)
        {
            return E_NOTIMPL;
        }

        HRESULT STDMETHODCALLTYPE
            GetPriority(LONG* /*pnPriority*/)
        {
            return E_NOTIMPL;
        }

        HRESULT STDMETHODCALLTYPE
            OnLowResource(DWORD /*reserved*/)
        {
            return E_NOTIMPL;
        }

        HRESULT STDMETHODCALLTYPE
            OnProgress(ULONG   ulProgress,
                ULONG   ulProgressMax,
                ULONG   ulStatusCode,
                LPCWSTR /*szStatusText*/)
        {
            switch (ulStatusCode)
            {
            case BINDSTATUS_BEGINDOWNLOADDATA:
                isDownloading = true;
                break;
            case BINDSTATUS_DOWNLOADINGDATA:
            case BINDSTATUS_ENDDOWNLOADDATA:
            {
                int percent = (int)(100.0 * static_cast<double>(ulProgress)
                    / static_cast<double>(ulProgressMax));
                if (m_percentLast < percent)
                {
                    percent = percent;
                    m_percentLast = percent;
                }
                if (ulStatusCode == BINDSTATUS_ENDDOWNLOADDATA)
                {
                    isDownloading = false;
                }
            }
            break;

            default:
            {
                isDownloading = false;
            }
            }

            return S_OK;
        }

        HRESULT STDMETHODCALLTYPE
            OnStopBinding(HRESULT /*hresult*/,
                LPCWSTR /*szError*/)
        {
            return E_NOTIMPL;
        }

        HRESULT STDMETHODCALLTYPE
            GetBindInfo(DWORD*    /*grfBINDF*/,
                BINDINFO* /*pbindinfo*/)
        {
            return E_NOTIMPL;
        }

        HRESULT STDMETHODCALLTYPE
            OnDataAvailable(DWORD      /*grfBSCF*/,
                DWORD      /*dwSize*/,
                FORMATETC* /*pformatetc*/,
                STGMEDIUM* /*pstgmed*/)
        {
            return E_NOTIMPL;
        }

        HRESULT STDMETHODCALLTYPE
            OnObjectAvailable(REFIID    /*riid*/,
                IUnknown* /*punk*/)
        {
            return E_NOTIMPL;
        }
    };
}
