    /** URLキャッシュ ---------------------------------------------------------*/
    /** URLキャッシュ */
    private static Vector HISTORY         = new Vector();
    /** URLキャッシュ-カレント位置 */
    private static int    CURRENT_HISTORY = -1;
    /** データキャッシュ - キャッシュID - 最近アクセスした順にソート済み */
    private static Vector CACHE_ID        = new Vector();
    /** データキャッシュ - キャッシュデータ - 最近アクセスした順にソート済み
        配列の中身 0 データ
                   1 コンテントタイプ
                   2 コンテント長
                   3 圧縮前コンテント長
    */
    private static Vector CACHE_CONTENT   = new Vector();
    /** 読み込んだデータ */
    private static Object[] _browserCurrentContent;

    /**
        getContent
            キャッシュ又はインターネットからデータを取得する
 
        @param - 接続に失敗した場合は NULL
               - 接続に成功した場合は データ配列
    
        改造前  18843
    */
    private static final boolean getContent(
        String url,
        String method,
        String encodedValues,
        boolean html) {
        ASSERT(url != null);
        ASSERT(method.equals(HttpConnection.GET) || method.equals(HttpConnection.POST));
        ASSERT(encodedValues != null);
        TRACE("URL           = \"" + url + "\"");
        TRACE("METHOD        = " + method);
        TRACE("encodedValues = " + encodedValues + "," + URLDecoder.decode(encodedValues));

        for (;;) {
            /// キャッシュキーを取得する
            String cacheid = url + "\t" + method + "\t"+ encodedValues;

            /// キャッシュからデータを参照
            /// @warning SCB_BROWSER_MODE != BROWSER_MODE_STANDARDの場合
            /// 下記コードは不要だが条件判定文を削除する
            /// ためにカットしました。
            ASSERT(cacheid != null);
            ASSERT(CACHE_ID.size() == CACHE_CONTENT.size());
            int index = CACHE_ID.indexOf(cacheid);
            if (index != -1) {
                _browserCurrentContent = (Object[])CACHE_CONTENT.elementAt(index);
                ASSERT(_browserCurrentContent.length == 4);
            
                /// キャッシュ順位変更
                CACHE_ID.removeElementAt(index);
                CACHE_ID.insertElementAt(cacheid,0);
                CACHE_CONTENT.removeElementAt(index);
                CACHE_CONTENT.insertElementAt(_browserCurrentContent,0);
                if (html) {
                    CURRENT_STATUS = GETMSTR(IDS_0PACKET);
                }
                return true;
            }

            String urlProxy = URL_BROWSER_PROXY;
            if (!_confDisplayImage) {
                urlProxy += "-";
            }

            urlProxy += url;
            boolean get = method.equals(HttpConnection.GET);
            byte[] post = null;
            if (get && encodedValues.length() != 0) {
                urlProxy += "?" + encodedValues;
            }

            if (!get) {
                post = encodedValues.getBytes();
            }
            byte[] downloaded = download(urlProxy,post);
            if (downloaded == null) {
                return false;
            }
            /// ヘッダ情報を取得する
            String header   = _httpContentType;
            TRACE("header=" + header);
            int split1      = header.indexOf(";");
            int split2      = header.indexOf(";",split1 + 1);
            int length      = _httpContentLength;
            String contentType = header.substring(0,split1);
            String proxyStatus = header.substring(split1+1,split2);
            //URL
            String header3     = header.substring(split2+1);

            int status         = Integer.parseInt(proxyStatus);
            //  エラー処理
            if (600 <= status) {
                // _httpResponseからURLを切り出す
                String body       = new String(downloaded);
                int splitpos      = body.indexOf(";");
                String errmessage = body.substring(0,splitpos);
                String errurl     = body.substring(splitpos+1);
                if (IApplication.getCurrentApp().getLaunchType() != IApplication.LAUNCHED_AS_CONCIERGE) {
                    //  通常起動時はネイティブブラウザキック
                    if (showDialog(GETMSTR(IDS_IMODE_BROWSER_START),Dialog.DIALOG_YESNO) 
                        == Dialog.BUTTON_YES) {
                        IApplication.getCurrentApp().launch(IApplication.LAUNCH_BROWSER,new String[]{errurl});
                    }
                }
                //  アプリブラウザではアクセスできない
                showDialog(errmessage,Dialog.DIALOG_ERROR);
                return false;
            }
            /// リダイレクト処理
            if (300 <= status && status < 400 ) {
                url           = header3;
                method        = HttpConnection.GET;
                encodedValues = "";
                continue;
            }
            int olength = Integer.parseInt(header3);

            /// パケット節約履歴
            if (olength != 0 && html) {
                if (olength < length) {
                    length = olength;
                }
                CURRENT_STATUS = GETMSTR(IDS_PACKET_CUT) + ((olength-length) * 100 / olength) + "%";
                TOTAL_SAVED_PACKET_FEE += (olength - length + PACKET_SIZE -1) / PACKET_SIZE
                                          * CENTI_YEN_PER_PACKET / 100;
                saveConf();
            }

            /// 
            _browserCurrentContent = new Object[] {downloaded,
                                            contentType,
                                            new Integer(length),
                                            new Integer(olength)};

            /// キャッシュする
            if(_browserMode == BROWSER_MODE_STANDARD) {
                /// 最大キャッシュ数を超えた場合は、
                /// 古いキャッシュから削除する。
                int cache_size = CACHE_ID.size();
                if (MAX_CACHE_COUNT <= cache_size) {
                    TRACE("remove cache.");
                    cache_size--;
                    CACHE_ID.     removeElementAt(cache_size);
                    CACHE_CONTENT.removeElementAt(cache_size);
                }

                TRACE("cacheing...");
                CACHE_ID.     insertElementAt(cacheid,0);
                CACHE_CONTENT.insertElementAt(_browserCurrentContent,0);
                TRACE("cached");
            }
            return true;
        }
    }

    /**
     *  WEBサーバーに接続しドキュメントを要求する
     *  @param  url            接続先のURL
     *  @param  method         GETメソッドの場合-false/POSTメソッドの場合-true
     *  @pram   encodedValues  GET/POSTで送信する変数をエンコードしたもの
     *  @param  utn            UTN属性を通知するかどうか？
     */
    private static final void nextDocument(String url,String method,String encodedValues,boolean utn) {
        String requesturl = url;
        if (url.startsWith("https://") || url.startsWith("http://")) {
            requesturl = "!?"+URLEncoder.encode(url);
            if (url.indexOf("uid=NULLGWDOCOMO") != -1) {
                requesturl += "&" + "uid=NULLGWDOCOMO";
            }
        }
        if (!requestDocument(new String[]{requesturl,method,encodedValues},utn)) {
            return;
        }

        CURRENT_HISTORY++;

        // 将来の履歴を削除する
        int count   = HISTORY.size() - CURRENT_HISTORY;
        while (0 < count--) {
            HISTORY.removeElementAt(CURRENT_HISTORY);
        }

        // 履歴がMAX_HISTORY_COUNT以上になった場合古い履歴を一件削除
        if (MAX_HISTORY_COUNT < HISTORY.size()) {
            HISTORY.removeElementAt(0);
            CURRENT_HISTORY--;
        }

        HISTORY.addElement(new String[]{requesturl,method,encodedValues});

        repaint2();
    }


    /**
     *  ドキュメントをサーバーに要求する.
     *  @param request
     *         [0] url           URL
     *         [1] method        GET/POST
     *         [2] encodedValues formエンコードされた値
     *  @param utn true  の場合はUTNを通知をする.
     *             false の場合はUTNを通知をしない.
     */
    private static final boolean requestDocument(String[] request,boolean utn) {
        ASSERT(request != null);
        ASSERT(request.length == 3);

        //  リクエストするURLに端末IDを付与
        String url = request[0];
        if (utn) {
            if (showDialog("携帯電話情報を送信しますか?",Dialog.DIALOG_YESNO) == Dialog.BUTTON_YES) {
                // 普通のURLアクセス
                if (url.indexOf("!?http") != -1) {
                    url = "!%3A" + utnid + url.substring(1);
                // URLID によるアクセス
                } else {
                    url = url + "%3A" + utnid;
                }
            }
        }

        if (!getContent(url,request[1],request[2],true)) {
            showDialog(GETMSTR(IDS_BROWSER_REQUEST_ERROR),Dialog.DIALOG_ERROR);
            return false;
        }

        ASSERT(_browserCurrentContent != null);
        ASSERT(_browserCurrentContent.length == 4);

        String contentType = (String)_browserCurrentContent[1];
        byte[] content = (byte[])_browserCurrentContent[0];

        if (contentType.startsWith("image")) {
            disposeBrowserDocument();
            _documentImage = loadMediaI(content);
        } else if (contentType.startsWith("text")) {
            disposeBrowserDocument();
            parseDocument(content);
            formatDocument();
        } else {
            showDialog(GETMSTR(IDS_BROWSER_INVALID_CONTENT),Dialog.DIALOG_ERROR);
            return false;
        }
        return true;
    }

    /**
     *  ブラウザで閲覧中のドキュメントを削除する
     */
    private static final void disposeBrowserDocument() {
        unuseMedia(_documentImage);
        unuseMedia(_documentSound);
        _documentImage = null;
        _documentSound = null;
        _documentHtml  = null;
        disposeImageCache();
    }
