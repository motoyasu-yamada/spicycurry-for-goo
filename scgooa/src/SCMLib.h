    /**
     *  ダイアログを表示する.
     *  @param type  - ダイアログの種類を指定します.
     *  @param msg   - メッセージ文字列を指定します.
     *  @see    com.nttdocomo.ui.Dialog
     */
    private  static int showDialog(String msg,int type) {
        waitKeyRelease();
        Dialog dialog       = new Dialog(type,SystemMessage.DIALOG_TITLE);
#if defined(DEVICE_505)
        dialog.setFont(_font);
#endif
        dialog.setText(msg);
        int result = dialog.show();
        clearKeyEvent();
        repaint2();
        return result;
    }

    /** 入力された文字がallowedで渡された文字列からなるかどうかチェックする
        @param  tocheck チェック対象の文字列
        @return true  正規な文字列
                false 使ってはいけない文字が含まれている
    */
    private static boolean checkString(String allowed,String tocheck) {
        ASSERT(tocheck != null);
        for(int n = 0;n < tocheck.length();n++) {
            if (allowed.indexOf(tocheck.charAt(n)) == -1) {
                return false;
            }
        }
        return true;
    }

    /**
        ストリームから可変長文字列を読込む
        @param input ByteArrayInputStream を推奨する
        @return 文字列長が0の場合     - ""
                終端に達している場合  - null
        @warning このメソッド内でExceptionをキャッチする。
                 プログラム障害以外のユーザレベルの理由
                 (電波断)等でExceptionを生成するInputStream
                 を使ってはならない。
    */
    private static String readString16(InputStream input) {
        try {
            ASSERT(input != null);
            int length = readUI16(input);
            if (length == -1) {
                return null;
            }
            if (length == 0) {
                return "";
            }
            ASSERT(0 < length && length <= 0xffff);
            byte[] buffer = new byte[length];
            input.read(buffer,0,length);
            return new String(buffer);
        } catch(Throwable e) {
            DEBUG_ONLY(e.printStackTrace());
            ASSERT(false);
            return null;
        }
    }
    
    /**
    *   ストリームに可変長文字列を書込
    *   (文字列長 2バイト + NULL終端を含まないSJIS文字列
    */
    private static void writeString16(OutputStream output,String string) {
        ASSERT(string != null);
        ASSERT(output != null);
        try {
            //TRACE("writing = " + string);
            int length;
            byte[] buffer;
            buffer = getStringBytes(string);
            length = buffer.length;
            ASSERT(0<= length && length <= 0xffff);
            writeUI16(output,length);
            if (length == 0) {
                return;
            }
            output.write(buffer,0,length);
        } catch(Throwable e) {
            DEBUG_ONLY(e.printStackTrace());
            ASSERT(false);
        }
    }

    /**
    *   リトルエンディアンで16ビット整数値をストリームに書き込む
    */
    private static void writeUI16(OutputStream output,int n) {
        ASSERT(output != null);
        ASSERT(0<= n && n <= 0xffff);
        try {
            //TRACE("*** writeUI16 " + n);
            output.write(n & 0xff);
            output.write(n >> 8);
        } catch(Throwable e) {
            DEBUG_ONLY(e.printStackTrace());
            ASSERT(false);
        }
    }

    /**
    *   枠線つき文字列を描画する
    *   @param text   テキスト色
    *   @param border 枠色
    */
    private static void drawStringX(Graphics g,
                                    String s,
                                    int x,
                                    int y,
                                    int text,
                                    int border) {
        setColorOfRGB(g,border);
        g.drawString(s,x-1,y);
        g.drawString(s,x+1,y);
        g.drawString(s,x,y-1);
        g.drawString(s,x,y+1);
        setColorOfRGB(g,text);
        g.drawString(s,x,y);
    }

    /**
     *  スクラッチパッドに保存されているリソースから、
     *  文字列配列を取得します。
     *  @param     pos スクラッチパッド位置
     *  @exception このメソッドが投げる例外は事実上ありえません
     */
    private static String[] loadStrings(int pos) {
        ASSERT(0 <= pos && pos < MAX_SCRATCHPAD_LENGTH);
        try {
            InputStream input = Connector.openInputStream(SCRATCHPAD_URL + pos);
            int count = readUI16(input);
            String[] strings = new String[count];
            for (int n = 0;n < count;n++) {
                int length = readUI16(input);
                byte[] bytes = new byte[length];
                input.read(bytes,0,length);
                strings[n] = new String(bytes);
            }
            input.close();
            return strings;
        } catch(Exception e) {
            DEBUG_ONLY(e.printStackTrace());
            ASSERT(false);
            return null;
        }
    }

    /** スクラッチパッドに保存されているリソース内のブロック位置を取得します。
        @param     pos スクラッチパッド位置
        @exception このメソッドが投げる例外は事実上ありえません */
    private static int[] loadResources(int pos) {
        ASSERT(0 <= pos && pos < MAX_SCRATCHPAD_LENGTH);
        try {
            InputStream input = Connector.openInputStream(SCRATCHPAD_URL + pos);
            //  バージョンヘッダー読み飛ばし
            //  @warning,@knowhow SO505iは skip使うとその後の動作がおかしくなる。
            //input.skip(2);
            readUI16(input);
            int count = readUI16(input);
            int[] resources = new int[count];
            for (int n = 0;n < count;n++) {
                resources[n] = pos + readUI16(input);
            }
            input.close();
            return resources;
        } catch(Exception e) {
            DEBUG_ONLY(e.printStackTrace());
            ASSERT(false);
            return null;
        }
    }

    /** 
        リトルエンディアンでバックされた符号無し16ビット整数値を読み込む
        メーラー等のブラウザを呼び出すアプリケーションがこの関数を定義
        する場合 IMPORT_READ_UI16 を定義することで実装を排除することが
        できる。
        @param input ByteArrayInputStream を推奨する
        @warning このメソッド内でExceptionをキャッチする。
                 プログラム障害以外のユーザレベルの理由
                 (電波断)等でExceptionを生成するInputStream
                 を使ってはならない。
    */
    private static int  readUI16(InputStream input) {
        try {
            int n1;
            int n2;
            n1 = input.read();
            if (n1 == -1) {
                return -1;
            }
            n2 = input.read();
            if (n2 == -1) {
                return -1;
            }
            ASSERT(0 <= n1 && n1 <= 255);
            ASSERT(0 <= n2 && n2 <= 255);
            return  n1 + (n2 << 8);
        } catch(Exception e) {
            DEBUG_ONLY(e.printStackTrace());
            ASSERT(false);
            return -1;
        }
    }

    /** NULL終端SJIS文字列を読み込む
        @param reader SJISの読込元
        @return 読み込まれた文字列 空の場合は長さ0の文字列が返される
   */
    private static String readString(ByteArrayInputStream input) {
        try {
            ASSERT(input != null);
            byte[] bytes = new byte[10240];
            int length = 0;
            for (;;) {
                int c  = input.read();
                if (c == 0 || c == -1) {
                    return new String(bytes,0,length,"SJIS");
                }
                bytes[length++] = (byte)c;
            }
        } catch(Exception e) {
            DEBUG_ONLY(e.printStackTrace());
            ASSERT(false);
            return "";
        }
    }

    /**
        RGBで色を指定して描画色に設定する
        @param hex 0x00RRGGBB  0- 7 bits 青
                               8-15 bits 緑
                              16-23 bits 赤
    */
    private static final void setColorOfRGB(Graphics g,int hex) {
        g.setColor(Graphics.getColorOfRGB((hex >> 16) & 0xff,(hex >> 8) & 0xff,hex & 0xff));
    }

    /**
     *  文字を左揃えにする.
     *  @see drawText
     */
    private static final int ALIGN_LEFT   = 0;
    /**
     *  文字を右揃えにする.
     *  @see drawText
     */
    private static final int ALIGN_RIGHT  = 1;
    /**
     *  文字を中央揃えにする.
     *  @see drawText
     */
    private static final int ALIGN_CENTER = 2;

    /**
     *  テキストを矩形の中に描画する
     *  @param  text        描画するテキスト
     *  @param  x           矩形の左上のX座標
     *  @param  y           矩形の左上のY座標
     *  @param  w           矩形の幅
     *  @param  h           矩形の高
     *  @param  space       矩形の端から文字列までの間隔
     *  @param  multiLine   複数行描画するか
     *  @see    _font           描画に使うフォント
     *  @see    setColorOfRGB   描画に使い色を予め設定する
     */
    private static final void drawText(Graphics g,String text,int x,int y,int w,int h,int space,boolean multiLine,int align) {
        if (text == null) {
            return;
        } 
        int length = text.length();
        if (length == 0) {
            return;
        }
        //  矩形の四辺から間隔をとる
        x += space;
        y += space;
        w -= space * 2;
        h -= space * 2;
        int bottom = y + h;
        //  一行の場合縦方向はセンタリングする、複数行の場合は上端ぞろえにする.
        if (!multiLine) {
            y += (h - _font.getHeight()) / 2;
        }

        int start  = 0;
        do {
            int hardBreak = text.indexOf("\n",start);
            if (hardBreak == -1) {
                hardBreak = length;
            }
            int softBreak = _font.getLineBreak(text,start,hardBreak-start,w);
            String line;
            if (softBreak != -1 && softBreak < hardBreak) {
                line     = text.substring(start,softBreak);
                start = softBreak;
            } else {
                line = text.substring(start,hardBreak);
                start = hardBreak + 1;
            }
            int left = x;
            switch(align) {
            case ALIGN_RIGHT:
                left += w - _font.stringWidth(line);
                break;
            case ALIGN_CENTER:
                left += (w - _font.stringWidth(line)) / 2;
                break;
            case ALIGN_LEFT:
                break;
            default:
                ASSERT(false);
            }
            g.drawString(line,left,y + _font.getAscent());
            y += _font.getHeight();

        } while(multiLine && start < length && y <= bottom - _font.getHeight());
    }

    /**
     *  ティッカーを描画する.
     *
     *  @param text     文字列
     *  @param ticker   ティッカーカウント
     *  @param x    描画領域矩形の左上x座標
     *  @param y    描画領域矩形の左上y座標
     *  @param w    描画領域矩形の幅
     *  @param h    描画領域矩形の高
     *  @param space 描画領域矩形内のマージン
     */
    private static void drawTicker(Graphics g,String text,int ticker,int x,int y,int w,int h,int space) {
        x += space;
        y += space;
        w -= space * 2;
        h -= space * 2;
        final int stringWidth = _font.stringWidth(text);
        //  ティッカー表示の必要なし
        if (stringWidth < w) {
            g.drawString(text,x,y + _font.getAscent());
            return;
        }

        //  ティッカー表示する
        final int tickerOut = 10;
        final int tickerIn  = stringWidth + tickerOut;
        final int tickerStop= w + tickerIn;
        final int tickerMax = tickerStop + 100;
        ticker %= tickerMax;

        int offset;
        String draw;
        if (tickerOut <= ticker && ticker < tickerIn){
            offset = - (ticker - tickerOut);
        } else if (tickerIn <= ticker && ticker < tickerStop){
            offset = w - (ticker - tickerIn);
        } else {
            offset = 0;
        }

        g.clipRect(x,y,w,h);
        if (offset < 0 ) {
            int start    = (offset) < 0 ? _font.getLineBreak(text,0,text.length(),-offset) : 0;
            int offset2  = offset + _font.stringWidth(text.substring(0,start));
            g.drawString(text.substring(start),x + offset2 ,y + _font.getAscent());
        } else {
            g.drawString(text,x + offset ,y + _font.getAscent());
        }
        g.clearClip();
    }

    /**
     *  矩形を描画する
     *  @param  x           矩形の左上
     *  @param  y           矩形の左上
     *  @param  w           矩形の幅
     *  @param  h           矩形の高
     */
    private static final void drawBox(Graphics g,int x,int y,int w,int h,int bkgnd,int frame) {
        setColorOfRGB(g,bkgnd);
        g.fillRect(x,y,w,h);
        setColorOfRGB(g,frame);
        g.drawRect(x,y,w-1,h-1);
    }

    /**
     *   バイト配列からイメージオブジェクトを取り出す
     *   @param data  バイト配列画像データ
     */
    private  static MediaImage loadMediaI(byte[] data) {
#if defined(DEVICE_505)
        try {
            MediaImage mi = MediaManager.getImage(data);
            mi.use();
            return mi;
        } catch (Throwable e) {
            DEBUG_ONLY(e.printStackTrace());
            return null;
        }
#else
        FileSystem.writeScratchpad(SCRATCH_TEMPORARY,data,0,data.length);
        return loadMediaI(SCRATCH_TEMPORARY);
#endif
    }

    /**
    *   スクラッチパッドから画像を読み込む
    *   @param pos スクラッチパッド位置
    */
    private  static MediaImage loadMediaI(int scpos) {
        try {
            MediaImage mi = MediaManager.getImage(SCRATCHPAD_URL+scpos);
            mi.use();
            return mi;
        } catch (Throwable e) {
            DEBUG_ONLY(e.printStackTrace());
            return null;
        }
    }

    /**
     *  文字列をSJIS換算で指定バイト数以内で取得する.
     *  @param tmpString 文字列
     */
    private static byte[] getStringBytes(String tmpString) {
        ASSERT(tmpString != null);
         if(tmpString.length()==0) {
            return new byte[0];
         }
         return tmpString.getBytes();
    }

    /**
     *  SJIS換算で指定したバイト数に収まるように文字列を短くする.
     *  @param maxlength SJIS換算のバイト数
     */
    private static final String leftByBytes(String src,int maxlength) {
        if (src == null) {
            return src;
        }
        byte[] sjis = src.getBytes();
        if (sjis.length <= maxlength) {
            return src;
        }
        int last = maxlength;
        if (0x81 <= sjis[last-1] && sjis[last-1] <= 0x9F) {
            last--;
        }
        return new String(sjis,0,last);
    }

    /**
     *  垂直方向の三角形を描画する
     *  @param x 頂点のX座標
     *  @param y 頂点のY座標
     *  @param h 底辺の1/2及び高さ
     *           正の場合は上向きの三角形
     *           負の場合は下向きの三角形
     */
    private static final void fillArrowV(Graphics g,int x, int y, int h) {
        int i = (h >= 0) ? 1 : -1;
        h = h * i;
        for(int n = 0; n < h; n++) {
            g.drawLine(x - n,y, x + n,y);
            y += i;
        }
    }

    /**
     *  水平方向の三角形を描画する
     *  @param x 頂点のX座標
     *  @param y 頂点のY座標
     *  @param w 底辺の1/2及び高さ
     *           正の場合は左向きの三角形
     *           負の場合は右向きの三角形
     */
    private static final void fillArrowH(Graphics g,int x,int y,int w) {
        int i = (w >= 0) ? 1 : -1;
        w = w * i;
        for(int n = 0; n < w; n++) {
            g.drawLine(x,y - n,x,y + n);
            x += i;
        }
    }

    /**
     *  日付をYY/MM/DD HH:NN:SS 形式でフォーマットする.
     *  @param 日付
     *  @return 文字列
     */
    private static final String format(Calendar calendar) {
        return Integer.toString(calendar.get(Calendar.YEAR))
                + "/"
                + padding(Integer.toString(calendar.get(Calendar.MONTH) + 1),2,'0')
                + "/"
                + padding(Integer.toString(calendar.get(Calendar.DATE)),2,'0')
                + " "
                + padding(Integer.toString(calendar.get(Calendar.HOUR_OF_DAY)),2,'0')
                + ":"
                + padding(Integer.toString(calendar.get(Calendar.MINUTE)),2,'0')
                + ":"
                + padding(Integer.toString(calendar.get(Calendar.SECOND)),2,'0');
    }

