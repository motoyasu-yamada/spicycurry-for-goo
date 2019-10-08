
class C extends Canvas
    implements MediaListener,
               ComponentListener,
               Runnable {
    #include "mincludes.h"
    #include "Event.h"

    /**
     *  初期ステータスメッセージ
     */
    private static String SCM_SCENE_INIT_MSG;
    /**
     *  設定:   フォントサイズ
     */
    private static int _confFontSize;
    /**
     *  設定:   ブラウザで画像を表示するかどうか
     */
    private static boolean _confDisplayImage;

    /**
     *  描画
     */
    public void paint(Graphics g) {
    }

    private static void _paint(Graphics g) {
        synchronized(_canvas) {
            g.lock();
            // EMU_PAINT_POINTING_DEVICE(g);
            try {
                g.setFont(_font);
                switch (_currentScene) {
                case SCENE_INITING:
                    setColorOfRGB(g,_colors[COLOR_BKGND]);
                    g.fillRect(0,0,_canvas.getWidth(),_canvas.getHeight());
                    drawStringX(g,SCM_SCENE_INIT_MSG,5,20,_colors[COLOR_TEXT],_colors[COLOR_TEXT_FRAME]);
                    break;
                case SCENE_BROWSER:
                    paintBrowser(g);
                    break;
                case SCENE_MAIN_MACHIUKE:
                    paintMainMachiuke(g);
                    break;
                case SCENE_MAIN:
                    paintMain(g);
                    break;
                case SCENE_GOOPASSPORTID:
                    paintSetGooPassportID(g);
                    break;
                case SCENE_IMG_PLAYER:
                    paintImgPlayer(g);
                    break;
                case SCENE_IMG_FOLDER:
                case SCENE_ICON_FOLDER:
                    paintList(g,_currentScene);
                    break;
                }
            } catch(Throwable t) {
                t.printStackTrace();
                showDialog(t.toString(),Dialog.DIALOG_ERROR);
            }
            g.unlock(true);
        }
    }

    /**
     *  イベント処理ループ内で実際にイベントを処理.
     *  @see _event
     */
    private static void _processEvent() {
        //  システムイベント処理
        for(;;) {
            int event = EVENT_NULL;
            synchronized(_sysEvent) {
                if (_sysEvent.size() != 0) {
                    TRACE("_sysEvent.size()="+_sysEvent.size());
                    event = ((Integer)_sysEvent.elementAt(0)).intValue();
                    _sysEvent.removeElementAt(0);
                }
            }
            if (event == EVENT_NULL) {
                break;
            }
            try {
                switch(event) {
                case EVENT_SYS_MODE_CHANGED:
                    activate();
                    break;
                case EVENT_SYS_WAKEUP_TIMER:
                    wakeupTimer();
                    break;
                case EVENT_SYS_FOLD_CHANGED:
                    foldChanged();
                    break;
                case EVENT_SYS_REPAINT:
                    _canvas.repaint2();
                    break;
                }
            } catch(Throwable t) {
                t.printStackTrace();
            }
        }

        //  キーイベント処理
        int key = keyEvent();
        if (key != EVENT_NULL) {
            boolean wait = true;
            switch(_currentScene) {
            case SCENE_BROWSER:
                processBrowser(key);
                return;
            case SCENE_MAIN:
                processMain(key);
                break;
            case SCENE_IMG_FOLDER:
                processImgFolder(key);
                break;
            case SCENE_IMG_PLAYER:
                processImgPlayer(key);
                break;
            case SCENE_GOOPASSPORTID:
                processSetGooPassportID(key);
                break;
            // m_murai  表示アイコン設定
            case SCENE_ICON_FOLDER:
                processIconFolder(key);
                break;
            }
            if (!wait) {
                return;
            }
            repeatKey();
        } else {
            switch(_currentScene) {
            case SCENE_BROWSER:
                break;
            }
            sleepThread(100);
        }
        if (_enableTicker) {
            _canvas.repaint2();
            _tickerCount += 8;
        }
    }

    /**
    *   文字列の左側に文字を生めて字数をそろえる
    *   @param  str この文字列の左側に paddingで設定した文字を詰めて文字長を lengthにする
    *   @param  length  長さ
    *   @param  padding 詰める文字
    *   @return 左側にpadding文字を詰めて文字列を右揃えにしたもの
    */
    private static String padding(String str,int length,char padding) {
        ASSERT(str.length() <= length);
        while (str.length() < length) {
            str = padding + str;
        }
        ASSERT(str.length() == length);
        return str;
    }

    /**
     *  スレッドを指定したミリ秒スリープさせる
     *  発生した例外は無視する。
     *  @param milliSeconds スリープするミリ秒数
     */
    private static void sleepThread(int milliSeconds) {
        if (milliSeconds <= 0) {
            return;
        }
        try {
            Thread.sleep(milliSeconds);
        } catch (Exception exception) {
        }
    }

    /**
     *  強制描画を実行する
     */
    static void repaint2() {
        _paint(_canvas.getGraphics());
    }

    /**
    *   待受画像 / 音声等をすべてスクラッチパッドから読み込みます
    */
    private static void loadMachiuke() {
        unuseMedia(MACHIUKE0);
        MACHIUKE0 = null;
        //  ランダムで待受画像を設定
        int num = _machiukeFileIDList.size();
        if (num != 0) {
            Random r = new Random();
            r.setSeed(System.currentTimeMillis());
            int select = Math.abs(r.nextInt()) % num;
            Integer id = ((Integer)_machiukeFileIDList.elementAt(select));
            FileSystem.mountDir(DIRI_IMG);

            byte[] data = FileSystem.readFile(id.intValue(),false);
            if (data != null) {
                MACHIUKE0 = loadMediaI(data);
            } else {
                _machiukeFileIDList.removeElement(id);
            }
        }
        System.gc();
    }

    /**
     *   読込中の各種メディアを破棄
     */
    private static void unuseMedia(MediaResource data) {
        try {
            if (data != null) {
                data.unuse();
                data.dispose();
            }
        } catch(Throwable e) {
            DEBUG_ONLY(e.printStackTrace());
            ASSERT(false);
        }
    }


    /**
     *  表示フォントサイズの変更
     *  @param n Font.SIZE_SMALL,SIZE_LARGE,SIZE_MEDIUM
     */
    private static void changeFont(int n) {
        _confFontSize = n;
#if defined(DEVICE_505)
        final int [] fontsizes = {Font.SIZE_TINY,Font.SIZE_SMALL,Font.SIZE_MEDIUM};
#else
        final int [] fontsizes = {Font.SIZE_SMALL,Font.SIZE_MEDIUM,Font.SIZE_LARGE};
#endif
        _font = Font.getFont(Font.FACE_SYSTEM | fontsizes[n] | Font.STYLE_PLAIN);
    }

    /**
     *   キャンバスをアクティベートする
     *   @param soft1 ソフトキー1
     *   @param soft2 ソフトキー2
     */
    private static void activateCanvas(String soft1,String soft2) {
        if (Display.getCurrent() != _canvas) {
            Display.setCurrent(_canvas);
        }
        _softkeys[0] = soft1;
        _softkeys[1] = soft2;
        _canvas.setSoftLabel(Frame.SOFT_KEY_1,soft1);
        _canvas.setSoftLabel(Frame.SOFT_KEY_2,soft2);
        _canvas.repaint2();
        clearKeyEvent();
    }

    /**
     *  画面タイトルを描画する.
     *  @param title タイトル
     *  @return タイトル領域の高さ
     */
    private static int paintTitle(Graphics g,String title) {
        int titleHeight = _font.getHeight() + 4;
        drawBox(g,0,0,_canvas.getWidth(),titleHeight,_colors[COLOR_TITLE_BKGND],_colors[COLOR_TITLE_FRAME]);
        setColorOfRGB(g,_colors[COLOR_TITLE_TEXT]);
        drawText(g,title,0,0,_canvas.getWidth(),titleHeight,2,false,ALIGN_CENTER);
        return titleHeight;
    }

    /**
     *  活性化処理を行う.
     */
    private static void activate() {
        stopTimer();
        protocolArrivalMailCount();
        openMain();
    }

    /**
     *  ウエイクアップタイマーで休眠から復帰し
     *  ニュース取得を行う
     */
    private static void wakeupTimer() {
        if(_currentScene != SCENE_MAIN_MACHIUKE) {
            return;
        }
        ASSERT(MAPPLICATION_MODE == MAPPLICATION_MODE_MACHIUKE);
        ASSERT(_currentScene     == SCENE_MAIN_MACHIUKE);
        // protocolGetTicker();

        //  ニュース取得
        protocolGetNews();
        repaint2();
        if(CURRENT_POLLING_TIME != 0) {
            MAPP.setWakeupTimer(CURRENT_POLLING_TIME * MINUTE_MILLIS);
        }
        if (FOLD_MODE == FOLD_MODE_CLOSE) {
            MAPP.sleep();
        }
    }

    /**
     *  フォールド開閉処理
     */
    private static void foldChanged() {
        if (FOLD_MODE != FOLD_MODE_CLOSE) {
            return;
        }
        loadMachiuke();
        openMainMachiuke();
        MAPP.sleep();
    }

} // C
