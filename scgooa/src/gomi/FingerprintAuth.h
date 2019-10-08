    private static boolean _confUseFingerprintAuth;

    /**
     *  起動認証を行う
     *  @return true  起動してもよい
     *          false 起動してはならない
     */
    private static boolean fingerprintAuth() {
        try {
            FingerprintAuthenticator auth = FingerprintAuthenticator.getFingerprintAuthenticator();
            return auth.authenticate() != -1;
        } catch(Exception e) {
            return false;
        }
    }

    /**
     *  指紋認証の設定UI
     *
     */
    private static void trackFingerprintAuthConf() {
        if (_confUseFingerprintAuth) {
            if (!fingerprintAuth()) {
                showDialog(GETMSTR(IDS_FINGERPRINT_AUTH_ERROR),Dialog.DIALOG_ERROR);
            }
        }
        int defaultSelect = _confUseFingerprintAuth ? IDM_FINGERPRINT_AUTH_ON : IDM_FINGERPRINT_AUTH_OFF;
        switch(trackMenu(null,loadStrings(MRESOURCES[IDM_FINGERPRINT_AUTH]),defaultSelect,true)) {
        case -1:
            break;
        case IDM_FINGERPRINT_AUTH_ON:
            if (fingerprintAuth()) {
                _confUseFingerprintAuth = true;
                saveConf();
                showDialog(GETMSTR(IDS_FINGERPRINT_AUTH_ON_DONE),Dialog.DIALOG_INFO);
            } else {
                showDialog(GETMSTR(IDS_FINGERPRINT_AUTH_ERROR),Dialog.DIALOG_ERROR);
            }
            break;
        case IDM_FINGERPRINT_AUTH_OFF:
            _confUseFingerprintAuth = false;
            saveConf();
            showDialog(GETMSTR(IDS_FINGERPRINT_AUTH_OFF_DONE),Dialog.DIALOG_INFO);
            break;
        }
    }

