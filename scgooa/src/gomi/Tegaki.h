#if defined(USE_POINTING_DEVICE)

    private static int _tegakiX = -1;
    private static int _tegakiY = -1;
    private static int _tegakiColor = 0x000000;
    private static int _tegakiBkgnd = 0xFFFFFF;
    private static Image _tegaki;
    private static int _tegakiToolY = 0;
    /**
     *  ペンサイズ
     */
    private static int _tegakiPenSize = 0;
    /**
     *  ペンスタイル
     */
    private static int _tegakiPenStyle = 0;
    /**
     *  編集済かどうか
     */
    private static boolean _tegakiModified;
    /**
     *  現在編集中のファイルID
     */
    private static int _tegakiFileID;
    /**
     *  戻る画面
     */
    private static int _tegakiReturnScene;

    /**
     *  手書きメモ画面を開く
     *  @param  open    編集したい画像のファイルIDを指定する.
     *                  新規に指定する場合は FILEID_INVALID を指定する.
     */
    private final static void openTegaki(int open,int returnScene) {
        _tegakiReturnScene = returnScene;
        FileSystem.mountDir(DIRI_IMG);
        _tegaki = Image.createImage(_canvas.getWidth(),_canvas.getHeight());
        if (open != FILEID_INVALID) {
            byte[] data = FileSystem.readFile(open,false);
            MediaImage image = loadMediaI(data);
            if (image == null) {
                showDialog(GETMSTR(IDS_UNLOADABLE_IMG),Dialog.DIALOG_ERROR);
                exitTegaki();
                return;
            }
            Graphics g = _tegaki.getGraphics();
            g.drawImage(image.getImage(),
                        (_canvas.getWidth()  - image.getWidth() ) / 2,
                        (_canvas.getHeight() - image.getHeight()) / 2);
            g.dispose();
            g = null;
            unuseMedia(image);
            image = null;
            data  = null;
            System.gc();
            _tegakiModified = false;
        } else {
            _tegakiModified = true;
        }

        _tegakiFileID = open;
        _currentScene = SCENE_TEGAKI;
        activateCanvas(GETMSTR(IDS_SFK_RETURN),GETMSTR(IDS_SFK_MENU));
    }

    /**
     *  手書き描画画面を描画する
     *
     */
    private final static void paintTegaki(Graphics g) {
        g.drawImage(_tegaki,0,0);

        int y = _tegakiToolY;
        
        //  背景描画
        setColorOfRGB(g,0x000080);
        g.fillRect(0,y,60,20);
        setColorOfRGB(g,0x000000);
        g.drawRect(0,y,60,20);

        //  ペンスタイル
        setColorOfRGB(g,0xFFFFFF);
        g.fillRect(2,y + 2,16,16);
        setColorOfRGB(g,0x000000);
        g.drawRect(2,y + 2,15,15);
        setColorOfRGB(g,0x000000);
        dotByPen(g, 10,y + 10,2,_tegakiPenStyle);
        drawArrow(g,10,y + 10,8,8);

        //  ペンの太さ
        setColorOfRGB(g,0xFFFFFF);
        g.fillRect(22,y + 2,16,16);
        setColorOfRGB(g,0x000000);
        g.drawRect(22,y + 2,15,15);
        setColorOfRGB(g,0x000000);
        dotByPen(g,30,y + 10,_tegakiPenSize,PEN_STYLE_CIRCLE);
        drawArrow(g,30,y + 10,8,8);

        //  ペンの色
        setColorOfRGB(g,_tegakiColor);
        g.fillRect(42,y + 2,16,16);
        setColorOfRGB(g,0xFFFFFF);
        g.drawRect(42,y + 2,15,15);
        setColorOfRGB(g,0x000000);
        g.drawRect(43,y + 3,13,13);
        drawArrow(g,50,y + 10,8,8);
    }

    /**
     *  元の画面に戻る
     */
    private final static void exitTegaki() {
        _tegaki.dispose();
        _tegaki = null;
        switch(_tegakiReturnScene) {
        case SCENE_IMG_FOLDER:
            openImgFolder();
            break;
        case SCENE_MAIN:
        default:
            openMain();
            break;
        }
    }

    /**
     *  手書き描画シーンのイベント処理
     *  @param  event   イベント番号
     */
    private final static void processTegaki(int event) {
        int cmd = -1;
        if (event == Display.KEY_SOFT1) {
            if (_tegakiModified) {
                if (showDialog(GETMSTR(IDS_TEGAKI_UNSAVE_CONFIRM),Dialog.DIALOG_YESNO) 
                    != Dialog.BUTTON_YES) {
                    return;
                }
            }
            exitTegaki();
            return;
        }

        if (event == EVENT_POINTER_SELECTED) {
            int x = POINTING_DEVICE_GET_X();
            int y = POINTING_DEVICE_GET_Y();
            //  ツールバーのクリック
            if (_tegakiToolY + 2 <= y && y < _tegakiToolY + 20 - 2 &&
                               2 <= x && x < 60 - 2) {
                int index = x / 20;
                if (0 <= index && index <= 2) {
                    cmd = index;
                }
                _tegakiX = -1;
                _tegakiY = -1;
            //  描画エリアのクリック
            } else {
                //  ツールバーの移動
                if (y < _canvas.getWidth() / 4) {
                    _tegakiToolY = _canvas.getWidth() - 20;
                } else if (_canvas.getWidth() * 3 / 4 < y) {
                    _tegakiToolY = 0;
                }
                //  描画
                Graphics tg = _tegaki.getGraphics();
                setColorOfRGB(tg,_tegakiColor);
                if (_tegakiX != -1 && _tegakiY != -1) {
                    drawLineByPen(tg,_tegakiX,_tegakiY,x,y,_tegakiPenSize,_tegakiPenStyle);
                } else {
                    dotByPen(tg,x,y,_tegakiPenSize,_tegakiPenStyle);
                }
                _tegakiModified = true;
                _tegakiX = x;
                _tegakiY = y;
            }
        } else {
            _tegakiX = -1;
            _tegakiY = -1;
        }
        if (event == Display.KEY_SOFT2) {
            switch (trackMenu(null,loadStrings(MRESOURCES[IDM_TEGAKI]),0,true)) {
            case IDM_TEGAKI_SEND_MAIL:
                if (_tegakiModified) {
                    if (showDialog(GETMSTR(IDS_TEGAKI_UNSAVE_CONFIRM),Dialog.DIALOG_YESNO) != Dialog.BUTTON_YES) {
                        return;
                    }
                }
                if (_tegakiFileID == FILEID_INVALID) {
                    showDialog(GETMSTR(IDS_TEGAKI_UNSAVE_ERROR),Dialog.DIALOG_ERROR);
                    return;
                }
                _tegaki.dispose();
                _tegaki = null;
                openMailEdit(SCENE_IMG_FOLDER,"","","",_tegakiFileID);
                break;
            case IDM_TEGAKI_SAVE:
                if (_tegakiFileID != FILEID_INVALID) {
                    FileSystem.writeFile(_tegakiFileID,jpeg(_tegaki));
                    _tegakiModified = false;
                    break;
                }
            case IDM_TEGAKI_SAVE_AS:
                _tegakiFileID = saveFile(jpeg(_tegaki));
                if (_tegakiFileID == FILEID_INVALID) {
                    showDialog(GETMSTR(IDS_TEGAKI_UNSAVE_ERROR),Dialog.DIALOG_ERROR);
                    return;
                }
                _tegakiModified = false;
                break;
            case IDM_TEGAKI_PEN_STYLE:
                cmd = 0;
                break;
            case IDM_TEGAKI_PEN_SIZE:
                cmd = 1;
                break;
            case IDM_TEGAKI_PEN_COLOR:
                cmd = 2;
                break;
            case IDM_TEGAKI_CLEAR:
                int select = trackColor(_tegakiBkgnd);
                if (select == -1) {
                    break;
                }
                _tegakiBkgnd = select;
                select = showDialog(GETMSTR(IDS_TEGAKI_CLEAR_CONFIRM),Dialog.DIALOG_YESNO);
                if (select == Dialog.BUTTON_YES) {
                    Graphics g = _tegaki.getGraphics();
                    setColorOfRGB(g,_tegakiBkgnd);
                    g.fillRect(0,0,_tegaki.getWidth(),_tegaki.getHeight());
                    _tegakiModified = true;
                }
                break;
            }
        }
        switch(cmd) {
        case 0:
            int style = trackMenu(null,loadStrings(MRESOURCES[IDM_PEN_STYLE]),_tegakiPenStyle,true);
            if (style != -1) {
                _tegakiPenStyle = style;
            }
            break;
        case 1:
            int size = trackMenu(null,loadStrings(MRESOURCES[IDM_PEN_SIZE]),_tegakiPenSize,true);
            if (size != -1) {
                _tegakiPenSize = size;
            }
            break;
        case 2:
            int color = trackColor(_tegakiColor);
            if (color != -1) {
                _tegakiColor = color;
            }
            break;
        }
        _canvas.repaint2();
    }


    /**
     *  色選択パレットを描画する
     *
     */
    private static int trackColor(int select) {
        Graphics g = _canvas.getGraphics();
        _canvas.setSoftLabel(Frame.SOFT_KEY_1,GETMSTR(IDS_SFK_RETURN));
        _canvas.setSoftLabel(Frame.SOFT_KEY_2,null);

        int cellw = _canvas.getWidth() * 9 / 10 / 18;
        int width  = cellw * 18;
        int height = cellw * 13;
        int left = (_canvas.getWidth()  - cellw * 18) / 2;
        int top = (_canvas.getHeight() - cellw * 13) / 2;

        clearKeyEvent();

        boolean loop = true;
        while (loop) {
            int event = keyEvent();
            if (event == EVENT_POINTER_SELECTED) {
                int px = (POINTING_DEVICE_GET_X() - left) / cellw;
                int py = (POINTING_DEVICE_GET_Y() - top) / cellw;
                if (1 <= py && py <= 12 && 0 <= px && px < 18) {
                    select = colorBy2D(px,py -1);
                } else if (py == 0 && 0 <=  px && px < 6) {
                    select = 0x333333 * px;
                } else {
                    select = -1;
                }
                loop = false;
            }
            if (event == Display.KEY_SOFT1) {
                select = -1;
                loop = false;
            }

            g.lock();
            setColorOfRGB(g,0x000040);
            g.fillRect(left - 4,top - 4,width + 8,height + 8);

            setColorOfRGB(g,0x000000);
            g.drawRect(left - 4,top - 4,width + 8,height + 8);

            setColorOfRGB(g,0xFFFF00);
            g.drawRect(left - 2,top - 2,width + 4,height + 4);

            for (int m = 0;m < 6;m++) {
                int color =  0x333333 * m;
                setColorOfRGB(g,color);
                g.fillRect(left + m * cellw + 2,top + 2,cellw - 4,cellw - 4);
            }
            for (int index = 0;index < 216;index++) {
                int cx = index % 18;
                int cy = index / 18;
                int color = colorBy2D(cx,cy);
                cx = left + cx * cellw ;
                cy = top + cy * cellw + cellw;
                if (select == color) {
                    setColorOfRGB(g,0xFFFF00);
                    g.fillRect(cx,cy,cellw,cellw);
                }
                setColorOfRGB(g,color);
                g.fillRect(cx + 2,cy + 2,cellw - 4,cellw - 4);
            }
            g.unlock(true);
        }
        clearKeyEvent();

        _canvas.setSoftLabel(Frame.SOFT_KEY_1,_softkeys[0]);
        _canvas.setSoftLabel(Frame.SOFT_KEY_2,_softkeys[1]);

        return select;
    }

    /**
     *  2次元 カラーパレット上の位置からRGBコードを取得する
     *  @param  x 0-17
     *  @param  y 0-11
     *  @return RGB
     */
    private static int colorBy2D(int x,int y) {
        int r = x % 6;
        int g = x / 6 + (y / 6) * 3;
        int b = y % 6;
        return ((r << 16) + (g << 8) + b) * 51;
    }

    static final int PEN_STYLE_CIRCLE = 0;
    static final int PEN_STYLE_RECT   = 1;
    static final int PEN_STYLE_DOWN   = 2;
    static final int PEN_STYLE_UP     = 3;

    /**
     *  直線を描画する
     *  ブレゼンハムアルゴリズムで描画する
     */
    final static void drawLineByPen(Graphics g,int x0, int y0, int x1, int y1,int penSize,int penStyle){
        int x = x0;
        int y = y0;
        int wx = Math.abs(x0-x1);
        int wy = Math.abs(y1-y0);
        int dx = (x0<x1)?1:-1;
        int dy = (y0<y1)?1:-1;
    
        // 傾きが1より小さい？
        if(wx >= wy){
            int E = -wx;
            while(x != x1){
                dotByPen(g,x,y,penSize,penStyle);
                x += dx;
                E += 2*wy;
                if(E >= 0){
                    E -= 2*wx;
                    y += dy;
                }
            }
        // 傾きが1より大きい場合(or-1より小さい場合)
        }else{ 
            int E = -wy;
            while(y != y1){
                dotByPen(g,x,y,penSize,penStyle);
                y += dy;
                E += 2*wx;
                if(E >= 0){
                    E -= 2*wy;
                    x += dx;
                }
            }
        }
    }

    /**
     *  ペンで点を描画する
     */
    static final void dotByPen(Graphics g,int x,int y,int size,int style) {
        int w  = new int[]{2,4,8}[size];
        int hw = w / 2;
        switch(style) {
        case PEN_STYLE_CIRCLE:
            g.fillArc(x - hw,y - hw,w,w,0,360);
            break;
        case PEN_STYLE_RECT:
            g.fillRect(x - hw,y - hw,w,w);
            break;
        case PEN_STYLE_DOWN:
            for (int n = 0;n <= size + 1;n++) {
                g.drawLine(x -hw,y - hw + n,x + hw,y + hw + n);
            }
            break;
        case PEN_STYLE_UP:
            for (int n = 0;n <= size + 1;n++) {
                g.drawLine(x + hw,y- hw + n,x - hw,y + hw + n);
            }
            break;
        default:
            throw new IllegalArgumentException();
        }
    }

    /**
     *  下向き矢印を描画する
     */
    private static void drawArrow(Graphics g,int x,int y,int w,int h) {
        int i1 = w / 3;
        int cx = x + w / 2;
        int drawingy = y + h / 2 + i1 / 2;
        
        for (int drawingw = 0; drawingw < i1; drawingw++) {
            g.drawLine(cx - drawingw, drawingy, cx + drawingw, drawingy);
            drawingy--;
        }
    }

    /**
     *  内部形式画像をJPEG形式に変換する
     *  @param i
     *  @return
     */
    private static final byte[] jpeg(Image i) {
        try {
            ByteArrayOutputStream output = new ByteArrayOutputStream();
            ImageEncoder encoder = ImageEncoder.getEncoder("JPEG");
            InputStream input = encoder.encode(i,0,0,i.getWidth(),i.getHeight()).getInputStream();          
            int n;
            while ((n = input.read()) != -1) {
                output.write(n);
            }
            return output.toByteArray();
        } catch(Exception e) {
            return null;
        }
    }

#endif
