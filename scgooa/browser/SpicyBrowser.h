#include "HtmlBreakLine.h"
#include "HtmlNode.h"
#include "DEBUGS.h"

/** 整数変数集合 */
private static int[] INT_PARAMS = new int[24];
#define  PACKET_SIZE 128
#define INT_PARAM(num)  (INT_PARAMS[num])
    /** 整形結果 - 垂直サイズ */
#define  DOCUMENT_HEIGHT            INT_PARAM(5)
    /** 整形用 - グローバル - 行幅 */
#define FORMATING_WIDTH_LINE        INT_PARAM(8)
    /** 整形用 - グローバル - 行高 */
#define FORMATING_HEIGHT_LINE       INT_PARAM(9)
    /** 整形用 - グローバル - 垂直位置 */
#define FORMATING_Y                 INT_PARAM(10)
    /** 整形用 - グローバル - 整列位置 */
#define FORMATING_ALIGN             INT_PARAM(11)

    /** 操作処理 - スクロール補正 */
#define SCROLL                      INT_PARAM(18)
    /** 操作処理 - アクティブなインタラクティブ */
#define ACTIVE_INTERACTIVE_INDEX    INT_PARAM(19)
    /** 累計節約パケット料金 */
#define TOTAL_SAVED_PACKET_FEE      INT_PARAM(21)
    /** */
#define PREV_FORMATING_ALIGN        INT_PARAM(23)
    /** 描画環境 - クライアント上辺 */
#define CLIENT_TOP                  0
    /** 描画環境 - クライアント左辺 */
#define CLIENT_LEFT                 0
#define DEFAULT_IMAGE_SIZE (PIXEL_OR_PERCENT_0PIXEL + 8)
#define MIN_SELECT_WIDTH                48
    /** 1パケット当りの金額 単位は1/100円 */
#define CENTI_YEN_PER_PACKET        30

    /**
    *   モジュールインクルード
    */
    //  レイアウトモジュール
    #include "SCBFormat.h"
    //  描画モジュール
    #include "SCBDraw.h"
    //  Http通信モジュール
    #include "SCBHttp.h"
    //  解析モジュール
    #include "SCBParse.h"
    //  操作モジュール
    #include "SCBSelect.h"

    /**
     *  ブラウザ起動モード - 通常
     */
    private static final int BROWSER_MODE_STANDARD = 1;
    /**
     *  ブラウザ起動モード - 全てのメニューをオフにしたインラインアクセス
     */
    private static final int BROWSER_MODE_INLINE = 2;

    /**
     *  解析結果 - HTMLドキュメント木構造のルートノード
     */
    private static HtmlNode   _documentHtml;
    /**
     *  音声ファイル実体
     */
    private static MediaSound _documentSound;
    /**
     *  画像ファイル実体
     */
    private static MediaImage _documentImage;

    /** 解析結果 - インタラクティブ */
    private static Vector     INTERACTIVES;
    /** 解析結果 - アクセスキー対応インタラクティブ集合 */
    private static HtmlNode[] ACCESSKEYS;
    /** 整形用 - グローバル */
    private static HtmlBreakLine FORMATING_BREAK_CURRENT;
    /**
     *  解析結果 - htmlタイトル
     */
    private static String _documentTitle;
    /** 高速率 / 圧縮率等の表示 */
    private static String CURRENT_STATUS;
    /** 現在表示中のページのURL インターネットURL*/
    private static String CURRENT_URL;
    /** 現在表示中のページのURL プロキシURL*/
    private static String LAST_URL;
    /** ブックマークタイトル*/
    public static Vector BOOKMARK_TITLE = new Vector();
    /** ブックマークURL */
    public static Vector BOOKMARK_URL   = new Vector();
    /**
     *  ブラウザの動作モード
     */
    private static int _browserMode;
    /**
     *  ブラウザを終了時に戻る画面
     */
    private static int _browserReturnScene;

    /**
     *  ブラウザを開く前のソフトキーの状態をキャッシュ
     */
    private static String[] _browserSavedSoftkeys = new String[2];
    /**
     *  ブラウザのソフトキー
     */
    private static final String[] _browserSoftkeys = new String[2];
    /**
     *  タッチパネルがドラッグされたかどうか
     */
    //private static boolean _pointerDrag;
    /**
     *  タッチパネルがドラッグされた場合のドラッグ開始位置
     */
    //private static int _pointerDragY;

    /**
     *  ブラウザを呼び出す
     *  @param mode       ブラウザ呼び出しモード
     *  @param notifyUID  UIDを通知するかどうか
     *  @param url        ブラウザが最初に表示するURL
     */
    private static void openBrowser(int mode,String url) {
        
        _browserReturnScene   = _currentScene;
        _browserSavedSoftkeys[0] = _softkeys[0];
        _browserSavedSoftkeys[1] = _softkeys[1];
        if (_currentScene == SCENE_RETRIEVE) {
            _browserReturnScene   = SCENE_MAIN;
            _browserSavedSoftkeys[0] = GETMSTR(IDS_CONFIG);
            _browserSavedSoftkeys[1] = GETMSTR(IDS_SFK_END);
        }

        //  URL直接入力モード
        if(url.equals("sb:inputurl")) {
            CURRENT_URL     = "http://";
            url = inputURL();
            if ("http://".equals(url) || "https://".equals(url)) {
                url = null;
            }
        //  ブックマークからURLを選択
        } else if (url.equals("sb:bookmark")) {
            url = selectFromBookmarkList();
        }

        if (url == null) {
            closeBrowser();
            return;
        }

        _browserMode = mode;
        _currentScene = SCENE_BROWSER;
        _browserSoftkeys[0] = GETMSTR(IDS_SFK_GOTOTOP);
        _browserSoftkeys[1] = mode == BROWSER_MODE_STANDARD ? GETMSTR(IDS_SFK_MENU) : null;
        activateCanvas(_browserSoftkeys[0],_browserSoftkeys[1]);

        CURRENT_STATUS = null;

        // 指定したURLにアクセスする
        nextDocument(url,HttpConnection.GET,"",false);
        if (CURRENT_HISTORY == -1) {
            closeBrowser();
            return;
        }
    }

    /**
     *  ブラウザを終了する
     */
    private static void closeBrowser() {
        disposeImageCache();
        CURRENT_HISTORY = -1;
        HISTORY.      removeAllElements();
        CACHE_ID.     removeAllElements();
        CACHE_CONTENT.removeAllElements();
        _browserCurrentContent = null;
        disposeBrowserDocument();
        _currentScene = _browserReturnScene;
        activateCanvas(_browserSavedSoftkeys[0],_browserSavedSoftkeys[1]);
    }

    /**
     *  メール編集/アドレス帳編集からブラウザに戻る
     */
//    private static void returnBrowser(){ 
//        _currentScene = SCENE_BROWSER;
//        activateCanvas(_browserSoftkeys[0],_browserSoftkeys[1]);
//    }
    
    /**
     *   @warning returnBrowserの処理と対応しています。
     *   @warning returnBrowserメソッドでSCM_PASS_EVENT_TO_BROWSERがtrueに
     *            なるまでこのメソッドは待機します。
     */
    private static void mailto(String mailto,String subject,String body) {
//        openMailEdit(SCENE_BROWSER,mailto,subject,body,FILEID_INVALID);
        return;
    }

    /**
     *  ブラウザ描画画面
     */
    private static void paintBrowser(Graphics g) {
        //  画像形式のコンテンツ
        if (_documentImage != null) {
            //  背景消去
            drawBox(g,0,0,_canvas.getWidth(),_canvas.getHeight(),_colors[COLOR_BKGND],_colors[COLOR_FRAME]);
            //  画像描画
            int x = (_canvas.getWidth()   - _documentImage.getWidth()) / 2;
            int y = (_canvas.getHeight()  - _documentImage.getHeight()) / 2;
            g.drawImage(_documentImage.getImage(),x,y);

        //  音声形式のコンテンツ
        } else if (_documentSound != null) {
            //  背景消去
            drawBox(g,0,0,_canvas.getWidth(),_canvas.getHeight(),_colors[COLOR_BKGND],_colors[COLOR_FRAME]);
            //  再生ステータスの描画
            String status = GETMSTR(_mediaStatus == MEDIA_STATUS_PLAYING ? IDS_SND_PLAY : IDS_SND_STOP);
            drawStringX(g,
                        status,
                        (_canvas.getWidth()  - _font.stringWidth(status)) / 2,
                        (_canvas.getHeight() - _font.getHeight()) / 2 + _font.getAscent(),
                        _colors[COLOR_TEXT],
                        _colors[COLOR_TEXT_FRAME]);

        //  HTMLを表示する
        } else if (_documentHtml != null) {
            _drawingX     = 0;
            _drawingY     = 0;
            _drawingYNext = 0;
            _drawingInAnchor  = false;
            _drawingVisibleTop    = SCROLL;
            _drawingVisibleBottom = SCROLL + _canvas.getHeight();
            _drawingFontNestDepth = 0;

            g.setFont(_font);
            g.setColor(DOCUMENT_COLOR_BG);
            g.fillRect(CLIENT_LEFT,CLIENT_TOP,_canvas.getWidth(),_canvas.getHeight());
            g.setOrigin(CLIENT_LEFT,CLIENT_TOP - SCROLL);
            drawNode(g,_documentHtml);
            g.setOrigin(0,0);
        }

        /// ステータスを表示する
        if (CURRENT_STATUS != null) {
            final int  lh = _font.getHeight() + 4;
            setColorOfRGB(g,_colors[COLOR_STAT_BKGND]);
            g.fillRect(0,_canvas.getHeight() - lh,_canvas.getWidth(),lh);
            setColorOfRGB(g,_colors[COLOR_STAT_TEXT]);
            g.drawString(CURRENT_STATUS,2,_canvas.getHeight() - _font.getDescent() - 2);
            CURRENT_STATUS = null;
        }
    }

    /**
    *   ブラウザのHTML閲覧画面でのキーイベント処理
    *   18402
    */
    private static void processBrowser(int key) {
        if (key == Display.KEY_SOFT1) {
            closeBrowser();
            return;
        }

        if (key == Display.KEY_LEFT && 0 < CURRENT_HISTORY) {
            requestDocument((String[])HISTORY.elementAt(--CURRENT_HISTORY),false);
        }
        if (key == Display.KEY_RIGHT && CURRENT_HISTORY < HISTORY.size() - 1) {
            requestDocument((String[])HISTORY.elementAt(++CURRENT_HISTORY),false);
        }
        if (key == Display.KEY_DOWN) {
            scroll1(true);
        }
        if (key == Display.KEY_UP) {
            scroll1(false);
        }
        if (key == Display.KEY_ASTERISK ||
            key == Display.KEY_PAGE_UP) {
            scrollPage(- _canvas.getHeight());
        }
        if (key == Display.KEY_PAGE_DOWN ||
            key == Display.KEY_POUND) {
            scrollPage(_canvas.getHeight());
        }

        if (_documentHtml != null) {
            if (key == Display.KEY_SELECT) {
                select(getInteractive(ACTIVE_INTERACTIVE_INDEX));
            }
            if (Display.KEY_0 <= key && key <= Display.KEY_9) {
                select(ACCESSKEYS[key]);
            }
            if (key == Display.KEY_SOFT2) {
                if (_browserMode == BROWSER_MODE_STANDARD) {
                    menuBrowserMenu(trackMenu(null,loadStrings(SRES[IDM_BROWSER]),0,true));
                }
            }
        } else if (_documentImage != null) {
            switch(trackMenu(null,loadStrings(SRES[IDM_IMG_PLAYER_ATTACH]),0,true)) {
            case IDM_IMG_PLAYER_ATTACH_SAVE:
                FileSystem.mountDir(DIRI_IMG);
                saveFile((byte[])_browserCurrentContent[0]);
                break;
#if defined(USE_IMAGE_STORE)
            case IDM_IMG_PLAYER_ATTACH_EXPORT:
                try {
                    MediaImage image = MediaManager.getImage((byte[])_browserCurrentContent[0]);
                    image.use();
                    ImageStore.addEntry(image);
                    image.unuse();
                    image.dispose();
                    showDialog(GETMSTR(IDS_SAVE_SUCCESS),Dialog.DIALOG_INFO);
                } catch (Exception e) {
                    showDialog(GETMSTR(IDS_SAVE_ERROR),Dialog.DIALOG_ERROR);
                }
                break;
#endif
            }
        }
        _canvas.repaint2();
        repeatKey();
    }

    /**
    *   ブラウザの右ソフトキーで開くメニュー
    *   @param select ユーザが選択したメニューのID
    */
    private static void menuBrowserMenu(int select) {
        if (select == IDM_MAIN_HOME) {
            nextDocument(GETMSTR(IDS_BROWSER_HOME),HttpConnection.GET,"",false);
            return;
        }

        if (select ==  IDM_MAIN_INPUT_URL) {
            String url = inputURL();
            if (url != null) {
                nextDocument(url,HttpConnection.GET,"",false);
            }
            return;
        }

        if (select ==   IDM_MAIN_BOOKMARK) {
            String url = selectFromBookmarkList();
            if (url != null) {
                nextDocument(url,HttpConnection.GET,"",false);
            }
            return;
        }
        
        if (select == IDM_MAIN_BOOKMARK_ADD) {
            if (BOOKMARK_TITLE.size() < MAX_BOOKMARK_COUNT) {
                BOOKMARK_TITLE.insertElementAt(leftByBytes(_documentTitle,MAX_BOOKMARK_TITLE_LENGTH),0);
                BOOKMARK_URL  .insertElementAt(leftByBytes(CURRENT_URL,MAX_BOOKMARK_URL_LENGTH),0);
                saveBookmark();
                showDialog(GETMSTR(IDS_BOOKMARK_ADD_DONE), Dialog.DIALOG_INFO);
            } else {
                showDialog(GETMSTR(IDS_BOOKMARK_OVER_SIZE),Dialog.DIALOG_ERROR);
            }
            return;
        }
        if (select == IDM_MAIN_FONTSIZE) {
            int menuSelect;
            if ((menuSelect = trackMenu(null,loadStrings(SRES[IDM_FONTSIZE]),_confFontSize,true)) != -1) {
                changeFont(menuSelect);
                formatDocument();
                saveConf();
            }
            return;
        }
        if (select == IDM_MAIN_IMAGE) {
            int menuSelect = _confDisplayImage ? IDM_IMAGE_ON : IDM_IMAGE_OFF;
            if ((menuSelect = trackMenu(null,loadStrings(SRES[IDM_IMAGE]),menuSelect,true)) != -1) {
                _confDisplayImage = menuSelect == IDM_IMAGE_ON;
                saveConf();
            }
            return;
        }

        /// パケット履歴の閲覧
        if (select ==  IDM_MAIN_PACKETS) {
            showDialog(GETMSTR(IDS_TOTAL_SAVED_PACKET_FEE) +
                        TOTAL_SAVED_PACKET_FEE +
                        GETMSTR(IDS_YEN),
                        Dialog.DIALOG_INFO);
            return;
        }
        /// パケット履歴クリア
        if (select ==  IDM_MAIN_CLEAR_PACKETS) {
            showDialog(GETMSTR(IDS_CLEAR_TOTAL_PACKETS),Dialog.DIALOG_INFO);
            TOTAL_SAVED_PACKET_FEE = 0;
            return;
        }

        /// ページ再読込
        if (select == IDM_MAIN_RELOAD) {
            CACHE_ID.removeAllElements();
            CACHE_CONTENT.removeAllElements();
            requestDocument((String[])HISTORY.elementAt(CURRENT_HISTORY),false);
            return;
        }

        if (select == IDM_MAIN_DISPLAY_URL) {
            inputURL();
            return;
        }

///     GOO専用のカスタマイズ
        if (select == IDM_MAIN_SET_PRIVATE_ICON) {
            setPrivateIcon(CURRENT_URL);
            return;
        }
///
        ASSERT(select == -1);
    }

    /**
     *   ブックマーク一覧からURLを選択する
     */
    private static String selectFromBookmarkList() {
        TRACE("selectFromBookmarkList");

        /// ブックマークに一件も登録されていない場合
        if (BOOKMARK_TITLE.size() == 0) {
            showDialog(GETMSTR(IDS_BOOKMARK_EMPTY),
                       Dialog.DIALOG_ERROR);
            return null;
        }

        for (;;) {
            int select = trackMenu(null,BOOKMARK_TITLE,0,true);
            /// 左キーでキャンセル
            if (select == -1) {
                return null;
            }
            String bookmarkurl = (String)BOOKMARK_URL.elementAt(select);
            int menu  = trackMenu(null,loadStrings(SRES[IDM_BOOKMARK]),0,true);
            /// ブックマークを開く
            if (menu == IDM_BOOKMARK_OPEN) {
                return bookmarkurl;
            }
            /// ブックマークを削除する
            if (menu == IDM_BOOKMARK_DELETE) {
                if (showDialog(GETMSTR(IDS_BOOKMARK_DELETE_CONFIRM),
                               Dialog.DIALOG_YESNO) 
                    != Dialog.BUTTON_YES) {
                    continue;
                }
                BOOKMARK_URL.  removeElementAt(select);
                BOOKMARK_TITLE.removeElementAt(select);
                saveBookmark();
                showDialog(GETMSTR(IDS_BOOKMARK_DELETE_DONE),
                           Dialog.DIALOG_INFO);
                if (BOOKMARK_TITLE.size() == 0) {
                    return null;
                }
            }
            /// 全て削除する
            if (menu == IDM_BOOKMARK_DELETE_ALL) {
                if (showDialog(GETMSTR(IDS_BOOKMARK_DELETE_ALL_CONFIRM),
                               Dialog.DIALOG_YESNO) 
                    != Dialog.BUTTON_YES) {
                    continue;
                }
                BOOKMARK_URL.  removeAllElements();
                BOOKMARK_TITLE.removeAllElements();
                saveBookmark();
                saveConf();
                showDialog(GETMSTR(IDS_BOOKMARK_DELETE_ALL_DONE),
                           Dialog.DIALOG_INFO);
                return null;
            }
            /// ブックマークタイトルを変更する
            if (menu == IDM_BOOKMARK_CHANGE) {
                String title = inputString(
                                (String)BOOKMARK_TITLE.elementAt(select),
                                TextBox.KANA,
                                TextBox.DISPLAY_ANY,
                                MAX_BOOKMARK_TITLE_LENGTH);
                if (title != null) {
                    BOOKMARK_TITLE.setElementAt(title,select);
                    saveBookmark();
                    showDialog(GETMSTR(IDS_BOOKMARK_CHANGE_DONE),Dialog.DIALOG_INFO);
                }
            }
        }
    }

    /**
     *  接続先のURLを手動で入力する画面
     *  @return キャンセルした場合          null
     *          キャンセルしなかった場合    URL
     */
    private static String inputURL() {
        String url = inputString(CURRENT_URL,TextBox.ALPHA,TextBox.DISPLAY_ANY,255);
        if (url == null) {
            return null;
        }
        if (url.length() <= 7) {
            return null;
        }
        return url;
    }

    /**
     *  ノードが可視状態(表示領域内)かどうか？
     */
    private static boolean isVisible(HtmlNode node) {
        if (node == null) {
            return false;
        }

        final int y = node.y;
        final int t = SCROLL;
        final int b = SCROLL + _canvas.getHeight() - _font.getHeight();
        return t <= y && y <= b;
    }

    /** スクロールオフセットを変更 */
    private static void offsetScroll(boolean down,int n) {
        if (_documentHtml != null && DOCUMENT_HEIGHT <= _canvas.getHeight()) {
            TRACE_HTML_OPERATE("document is smaller than client");
            return;
        }
        if(down) {
            SCROLL += n;
        }
        if (!down) {
            SCROLL -= n;
        }
        SCROLL = Math.max(SCROLL,0);
        final int scrollheight = DOCUMENT_HEIGHT - _canvas.getHeight() + _font.getHeight();
        // TRACE_HTML_OPERATE("SCROLLHEIGHT = " + scrollheight);
        SCROLL = Math.min(SCROLL,scrollheight);
    }

    private static String getQueryStringValue(String queryString,String name) {
        queryString = "&" + queryString + "&";
        name        = "&" + name + "=";
        int start = queryString.indexOf(name);
        if (start == -1) {
            return null;
        }
        start += name.length();
        int end = queryString.indexOf("&",start);
        ASSERT(end != -1);
        return URLDecoder.decode(queryString.substring(start,end));
    }

    private static HtmlNode getInteractive(int n) {
        if (INTERACTIVES.isEmpty()) {
            return null;
        }
        return (HtmlNode)INTERACTIVES.elementAt(n);
    }

    /**
     *  次にインタラクティブなノードへ移動する
     *  @param down true   下へスクロールする
     *              false  上へスクロールする
     */
    private static HtmlNode nextInteractive(boolean down,boolean activate) {
        final int size = INTERACTIVES.size();
        if (size == 0) {
            TRACE("interactive");
            return null;
        }

        final int current =  ACTIVE_INTERACTIVE_INDEX;
        int next = current;
        if (down) {
            next++;
        } else {
            next--;
        }
        if (size <= next || next < 0) {
            return null;
        }

        HtmlNode node = getInteractive(next);
        if (activate) {
            ACTIVE_INTERACTIVE_INDEX = next;
        }
        return node;
    }

    /**
     *  画面スクロールする
     */
    private static void scrollPage(int move) {
        boolean down = 0 < move;
        move = Math.abs(move);
        offsetScroll(down,move);

        HtmlNode node = getInteractive(ACTIVE_INTERACTIVE_INDEX);
        for (;;) {
            if (isVisible(node)) {
                break;
            }
            node = nextInteractive(down,true);
            /// @warning *や#キーのスクロールで無限ループになってしまう
            if (node == null) {
                break;
            }
        }
    }

    /** 1行スクロールまたは次の要素をアクティブにする*/
    private static void scroll1(boolean down) {
        HtmlNode next = nextInteractive(down,false);

        if (isVisible(next)) {
            nextInteractive(down,true);
            return;
        }

        offsetScroll(down,_font.getHeight() * 3);

        if (isVisible(next)) {
            nextInteractive(down,true);
        }
    }

    /** POST/GETで送信する変数名・変数値をエンコードする */
    private static final String encodeValues(Hashtable values) {
        ASSERT(values != null);

        StringBuffer valuesEncoded = new StringBuffer();
        Enumeration e = values.keys();
        while (e.hasMoreElements()) {
            String key = (String) e.nextElement();
            if (key.startsWith(">")) {
                continue;
            }
            valuesEncoded.append(URLEncoder.encode(key))
            .append("=")
            .append(URLEncoder.encode((String)values.get(key)))
            .append("&");
        }
        if (valuesEncoded.length() == 0) {
            return "";
        }
        return valuesEncoded.toString().substring(0,valuesEncoded.length()-1);
    }

    /**
        input タグのデフォルト値を取得する
    */
    private static String defaultInputLabel(int type) {
        if (type == ATTRIBUTE_VALUE_INPUT_SUBMIT) {
            return GETMSTR(IDS_DEFAULT_SUBMIT);
        }
        if (type == ATTRIBUTE_VALUE_INPUT_RESET) {
            return GETMSTR(IDS_DEFAULT_RESET);
        }
        return "";
    }

    /**
        パネルにボタンを追加する
    */
    private static Button addButton(Panel panel,int id) {
        Button button = new Button(GETMSTR(id));
        button.setSize(panel.getWidth(),button.getHeight());
        panel.add(button);
        return button;
    }

    /** タグの文字列型属性値を取得する 取得できなかった場合デフォルト値 */
    private static String getAttributeStr(HtmlNode node,int attributeName,String defaultValue) {
        Object value = node.attributes.get(new Integer(attributeName));
        if (value == null) {
            return defaultValue;
        }
        return (String) value;
    }
    
    /** タグの文字列型属性値を取得する */
    private static String getAttributeStr(HtmlNode node,int attributeName) {
        return (String)node.attributes.get(new Integer(attributeName));
    }

    /** タグの整数値属性値を取得する */
    private static int getAttributeInt(HtmlNode node,int attributeName,int defaultValue) {
        Object value;
        if ((value  = node.attributes.get(new Integer(attributeName))) == null) {
            return defaultValue;
        }
        return ((Integer)value).intValue();
    }

    /** 属性値の範囲が0-100の場合は比率。101以上の場合は101を基底とした絶対値。
        0-100 0%-100%
        101-255 0-154 */
    private static int getAttributePixelOrPercent(HtmlNode node,int attributeName,int defaultValue) {
        int pixelorpercent;
        if ((pixelorpercent = getAttributeInt(node,attributeName,-1)) == -1) {
            pixelorpercent = defaultValue;
        }
        if (101 <= pixelorpercent) {
            return pixelorpercent - PIXEL_OR_PERCENT_0PIXEL;
        }
        return _canvas.getWidth() * pixelorpercent / 100;
    }

    /** タグの色形式の属性値を取得する */
    private static int getAttributeColor(HtmlNode node,int attributeName,int defaultValue) {
        Object value;
        if ((value = node.attributes.get(new Integer(attributeName))) == null) {
            return defaultValue;
        }
        int hex = ((Integer)value).intValue();
        return Graphics.getColorOfRGB((hex >> 16) & 0xff,(hex >> 8) & 0xff,hex & 0xff);
    }
