

    /**
     *   gooメールに届いているメール数を取得する
     */
    private static boolean protocolArrivalMailCount() {
        TRACE("gooPassportID=" + gooPassportID);
        //  gooPassportIDを持っていなかったら新着メールチェックしない
        if (gooPassportID.equals("0") || gooPassportID.equals("")) {
            ArrivalMailCount = "";
            #if defined(DEBUG)
                ArrivalMailCount = "設定されていない";
            #endif
            return true;
        }

        ByteArrayOutputStream output = new ByteArrayOutputStream();
        writeString16(output, gooPassportID);
        ByteArrayInputStream input = protocol(URL_ARRIVAL_MAIL_COUNT, output.toByteArray(), true);
        if (input == null) {
            ArrivalMailCount = GETMSTR(IDS_NEW_GOO_ARRIVAL_MAIL) + GETMSTR(IDS_CAN_NOT_GET_GOO_ARRIVAL_MAIL_COUNT);
            return false;
        }
        String status = readString16(input);

        //  不正なgooID
        if (status.equals("unknown")) {
            ArrivalMailCount = GETMSTR(IDS_UNKNOWN_GOO_USER_ARRIVAL_MAIL);
        //  システム障害（goo）
        } else if (status.equals("dberror")) {
            ArrivalMailCount = GETMSTR(IDS_DOWN_GOO_ARRIVAL_MAIL);
        //  有料メール会員
        } else if (status.indexOf("+") != -1) {
            ArrivalMailCount = " ";
        //  新着メール数
        } else {
            ArrivalMailCount = GETMSTR(IDS_NEW_GOO_ARRIVAL_MAIL) + "：" + status + "通";
        }

        return true;
    }

    /**
     *  1. バージョンアップ情報
     *  2. お知らせ情報
     *  3. ニュース情報
     *  を取得する。
     */
    private static boolean protocolGetNews() {
        ByteArrayOutputStream post = new ByteArrayOutputStream();
        writeString16(post,version);

        ByteArrayInputStream input = protocol(URL_GET_NEWS2,post.toByteArray(), true);
        if (input == null) {
            NEWS = NEWS_NULL;
            existsInformation = false;
            requireVersionup = false;
            showDialog(GETMSTR(IDS_GOO_NEWS_GET_FAIL),Dialog.DIALOG_INFO);
            return false;
        }

        requireVersionup  = readUI16(input) == '1';
        existsInformation = readUI16(input) == '1';
        if (existsInformation) {
            informationTitle = readString16(input);
            informationURL   = readString16(input);
        }

        int count = readUI16(input);
        NEWS = new String[count][2];
        for (int n = 0; n < count; n++) {
            NEWS[n]    = new String[2];
            String title = readString16(input);
            String url   = readString16(input);
            NEWS[n] = new String[]{title,url};
        }
        return true;
    }

    /**
    *   サーバーへの接続の共通コード
    *   @param url      URL
    *   @param upload   POSTアップロードするデータ
    *   @param silent   trueにするとデータダウンロードに失敗しても
    *                   エラーダイアログが出ない
    *   @return null - サーバーへの接続が失敗
    *                  サーバーからの応答コードが不正
    *           成功した場合はHTTP応答の内容を配列に格納して
    *           ByteArrayInputStreamストリームで返却。
    *   @see   download
    */
    private static ByteArrayInputStream protocol(
        String  url,
        byte[]  upload,
        boolean silent  ){
        ASSERT(url != null);
        ASSERT(url.length() < 250);
        // ASSERT(upload == null || upload.length < MAX_UPLOAD_LENGTH);
        ByteArrayInputStream input;
        byte[]               downloaded;
        int status;

        downloaded    = download(url,upload);
        if (downloaded == null) {
            if (!silent) {
                showDialog(GETMSTR(IDS_IMODE_CONNECTION_ERROR),Dialog.DIALOG_ERROR);
            }
            return null;
        }
#if defined(DEBUG)
        int pos = 0;
        while (pos < downloaded.length) {
            int startpos = pos;
            String line = "";
            String asci = "";
            for (int n = 0;n < 16;n++) {
                if (downloaded.length <= pos) {
                    break;
                }
                String temp = "0" + Integer.toHexString(downloaded[pos]);
                temp = temp.substring(temp.length() - 2,temp.length());
                line += temp + " ";
                asci += (char)downloaded[pos];
                pos++;
            }
            line += "                                              ";
            line  = line.substring(0,48);
            asci += "                ";
            asci  = asci.substring(0,16);
            String addr = "";
            addr  = "000" + Integer.toHexString(startpos);
            addr  = addr.substring(addr.length()-4,addr.length());
            TRACE(addr + ":" + line + ":" + asci);
        }
#endif
        input         = new ByteArrayInputStream(downloaded);
        status = input.read() & 0xFF;
        if (status == PROTOCOL_STATUS_SUCCESS) {
            return input;
        }

        String errmsg = readString16(input);
        if (!silent) {
            showDialog(errmsg,Dialog.DIALOG_ERROR);
        }
        return null;
    }
