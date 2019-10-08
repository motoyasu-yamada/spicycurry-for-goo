    /**
     *  サーバからデータをダウンロードする
     *  データをダウンロードできなかった場合
     *  @param   url      データのURL
     *  @param   sizehead データの頭2バイトにサイズヘッダがついている場合はtrue
     *  @param   output   出力データ
                          output != null の場合POST接続
                          output == null の場合GET接続
     *  @return  null - ダウンロードに失敗した場合
     *           データ配列
     */
    private static byte[] download(String url,byte[] output) {
        
        url = MAPP.getSourceURL() + url;
        TRACE("download.url = \"" + url +"\"");

        Frame frame = Display.getCurrent();
        frame.setSoftLabel(Frame.SOFT_KEY_1,"中断");
        if (frame == _canvas) {
            frame.setSoftLabel(Frame.SOFT_KEY_2,null);
        } else {
            frame.setSoftLabel(Frame.SOFT_KEY_2,"....");
        }
        _httpContentLength = -1;
        _httpDone     = false;
        _httpUrl      = url;
        _httpPost     = output;
        _httpResponse = null;
        _http         = null;
        new Thread(_canvas).start();
        Graphics g = null;
        if (frame == _canvas) {
            g = _canvas.getGraphics();
        }
        int scroll = 0;
        boolean loop = true;
        for (;;) {
            if (g != null) {
                g.lock();
                g.setFont(_font);
                String msg = (_httpContentLength == -1) ? "接続中" : "読込中";
                int height = _font.getHeight() + 2;
                setColorOfRGB(g,_colors[COLOR_STAT_BKGND]);
                g.fillRect(0,_canvas.getHeight() - height,_canvas.getWidth(),height);
                setColorOfRGB(g,_colors[COLOR_STAT_RBKGND]);
                g.fillRect(scroll - _canvas.getWidth(),
                           _canvas.getHeight() - height,
                           _canvas.getWidth(),
                           height);

                int x = (_canvas.getWidth() - _font.stringWidth(msg)) / 2;
                int y = _canvas.getHeight() - _font.getDescent() - 1;
                setColorOfRGB(g,_colors[COLOR_STAT_TEXT]);
                g.drawString(msg,x,y);
                //  接続中...
                g.unlock(true);
                scroll = (scroll+4) % (_canvas.getWidth() * 2);
            }
            
            if (!loop) {
                break;
            }
            sleepThread(50);
            if (keyEvent() == Display.KEY_SOFT1) {
                if (_http != null) {
                    try {
                        _http.close();
                    } catch(Exception e) {
                        e.printStackTrace();
                    }
                }
                clearKeyEvent();
                loop = false;
            }
            if (_httpDone) {
                loop = false;
            }
        }
        if (g != null) {
            g.dispose();
            g = null;
            repaint2();
        }

        byte[] response = _httpResponse;
        _http            = null;
        _httpUrl         = null;
        _httpPost        = null;
        _httpResponse    = null;

        frame.setSoftLabel(Frame.SOFT_KEY_1,_softkeys[0]);
        frame.setSoftLabel(Frame.SOFT_KEY_2,_softkeys[1]);
        System.gc();
        return response;
    }

    /**
     *  ダウンロード済のコンテントレングス.
     *  接続中の場合は-1.
     */
    private static int     _httpContentLength;
    /**
     *  コンテント長
     */
    private static String  _httpContentType;
    /**
     *  接続したいURL
     */
    private static String  _httpUrl;
    private static byte[]  _httpPost;
    /**
     *  HTTP通信の結果取得した応答データ
     */
    private static byte[]  _httpResponse;
    /**
     *  HTTP接続
     */
    private static HttpConnection _http;
    /**
     *  HTTP通信が完了したかどうか
     */
    private static boolean _httpDone;

    /**
     *  通信スレッド
     */
    public void run() {
        InputStream input = null;
        try {
            if (_httpPost == null || _httpPost.length == 0) {
                _http = (HttpConnection)Connector.open(_httpUrl,Connector.READ,true);
                _http.setRequestMethod(HttpConnection.GET);
            } else {
                _http = (HttpConnection)Connector.open(_httpUrl,Connector.READ_WRITE,true);
                _http.setRequestMethod(HttpConnection.POST);
                _http.setRequestProperty("Content-Type", "application/x-www-form-urlencoded");
                OutputStream post = _http.openOutputStream();
                post.write(_httpPost,0,_httpPost.length);
                post.close();
            }
            _http.connect();
            input = _http.openInputStream();
            if (input == null) {
                ASSERT(false);
                throw new Exception();
            }
            _httpContentType   = _http.getType();
            _httpContentLength = 0;
            ByteArrayOutputStream output = new ByteArrayOutputStream();
            byte[] tmp = new byte[1024];
            int b;
            for(;;) {
                int read =input.read(tmp,0,tmp.length);
                if (read == -1) {
                    break;
                }
                if (read != 0) {
                    output.write(tmp,0,read);
                    _httpContentLength += read;
                }
            }
            input.close();
            _http.close();
            _httpResponse = output.toByteArray();
        } catch(Throwable e) {
            e.printStackTrace();
            try {
                if (_http != null) {
                    _http.close();
                    _http = null;
                }
            } catch(Throwable e2) {
                e2.printStackTrace();
            }
            _httpPost = null;
        }

        _httpDone = true;
    }
