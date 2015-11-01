//",* CURLAUTH_NONE         - No HTTP authentication
//",* CURLAUTH_BASIC        - HTTP Basic authentication (default)
//",* CURLAUTH_DIGEST       - HTTP Digest authentication
//",* CURLAUTH_GSSNEGOTIATE - HTTP GSS-Negotiate authentication
//",* CURLAUTH_NTLM         - HTTP NTLM authentication
//",* CURLAUTH_DIGEST_IE    - HTTP Digest authentication with IE flavour
//",* CURLAUTH_NTLM_WB      - HTTP NTLM authentication delegated to winbind helper
//",* CURLAUTH_ONLY         - Use together with a single other type to force no
//",* CURLAUTH_ANY          - All fine types set
//",* CURLAUTH_ANYSAFE      - All fine types except Basic
{"AUTH_NONE",       ((unsigned long)0)},
{"AUTH_BASIC",       (((unsigned long)1)<<0)},
{"AUTH_DIGEST",      (((unsigned long)1)<<1)},
{"AUTH_GSSNEGOTIATE",(((unsigned long)1)<<2)},
{"AUTH_NTLM",        (((unsigned long)1)<<3)},
{"AUTH_DIGEST_IE",   (((unsigned long)1)<<4)},
{"AUTH_NTLM_WB",     (((unsigned long)1)<<5)},
{"AUTH_ONLY",        (((unsigned long)1)<<31)},
{"AUTH_ANY",         (~CURLAUTH_DIGEST_IE)},
{"AUTH_ANYSAFE",     (~(CURLAUTH_BASIC|CURLAUTH_DIGEST_IE))},
