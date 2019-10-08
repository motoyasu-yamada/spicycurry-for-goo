//  $Id: SCMMailBody.h,v 1.1.1.1 2005/03/04 10:31:16 m_yamada Exp $

/** 現在アクティブなアンカーはない */
#define ANCHOR_TYPE_INACTIVE 0
/** 現在アクティブなアンカーはURL */
#define ANCHOR_TYPE_URL      1
/** 現在アクティブなアンカーはメアド */
#define ANCHOR_TYPE_MAILTO   2
/** 現在アクティブなアンカーはメアド */
#define ANCHOR_TYPE_TELTO    3
/** 添付ファイル   */
#define ANCHOR_TYPE_ATTACH   4
/** メールの新規ページ取得 */
#define ANCHOR_TYPE_PAGE     5

    private final static int MEDIA_TYPE_IMAGE = 0;
    private final static int MEDIA_TYPE_SOUND = 1;
    /**
    *   現在閲覧中のメール
    */
    private static String     CURRENT_MAIL_MAIL;
    private static String     CURRENT_MAIL_SUBJECT;
    private static String     CURRENT_MAIL_DATE;
    private static String     CURRENT_MAIL_BODY;
    private static int        CURRENT_MAIL_READ_PAGE;
    private static int        CURRENT_MAIL_PAGE_COUNT;
    // 行ごとに配列に格納した本文
    private static Vector     CURRENT_MAIL_FORMATED;
    // 行ごとの開始位置を記録したもの
    private static Vector     CURRENT_MAIL_LINEPOS;
    // ドキュメント内のアンカー情報
    private static Vector     CURRENT_MAIL_ANCHORS_TYPE;
    private static Vector     CURRENT_MAIL_ANCHORS_TAG;
    private static Vector     CURRENT_MAIL_ANCHORS_RANGE;
    // 現在アクティブなアンカー 
    private static int        ACTIVE_ANCHOR;
    // 現在開いている添付ファイル 
    private static byte[]     CURRENT_ATTACH;
    // ダウンロード済みのページのページ番号リスト 
    private static Vector     DOWNLOADED_BODY_PAGENO;
    // ページ 
    private static Vector     DOWNLOADED_BODY_PAGE;
    /**
     *  メール本文を表示するかどうか？
     */
    private static boolean    _mailBodyRedrawBody;
    /**
     *  現在のメール本文のスクロール位置-画面最上位の行
     */
    private static int        _mailBodyScroll;
    /**
     *  現在表示中のページ番号 1 - ページ数
     */
    private static int _mailCurrentPageNo;

    /**
    *   メール本文閲覧画面を立ち上げる
    *       メールの整形処理を行う
    *   @param index メールインデックス
    *   @param body  メール本文
    *
    *   @see CURRENT_MAIL_TO
    *   @see CURRENT_MAIL_FROM
    *   @see CURRENT_MAIL_DATE
    *   @see CURRENT_MAIL_SUBJECT
    *   @see CURRENT_MAIL_BODY
    *   @see CURRENT_MAIL_FORMATED
    *   @see CURRENT_MAIL_ATTACHMENT
    */
    private static void openMailBody(int index,String body,int currentpage) {
        // 宛先,差出人,日付,本文,件名
        DEBUG_ONLY(validateML());
        CURRENT_MAIL_MAIL    = (String)ML_MAIL.elementAt(index);
        CURRENT_MAIL_DATE    = (String)ML_DATE.elementAt(index);
        CURRENT_MAIL_SUBJECT = (String)ML_SUBJECT.elementAt(index);
        CURRENT_MAIL_BODY    = body;
        ASSERT(CURRENT_MAIL_MAIL    != null);
        ASSERT(CURRENT_MAIL_DATE    != null);
        ASSERT(CURRENT_MAIL_SUBJECT != null);
        ASSERT(CURRENT_MAIL_BODY    != null);
        //  アンカー情報を初期化
        CURRENT_MAIL_ANCHORS_TYPE  = new Vector();
        CURRENT_MAIL_ANCHORS_TAG   = new Vector();
        CURRENT_MAIL_ANCHORS_RANGE = new Vector();
        //  ページ情報を初期化
        Vector lines   = new Vector();
        Vector linepos = new Vector();
        int offset = 0;

        //  スクロール位置をリセット
        _mailBodyScroll    = 0;

        //  添付ファイル / アンカー追加と本文に追加
        String line1;
        int    linelen;
        ASSERT(0<= index && index < ML_ATTCH.size());
        String[] attachments = (String[])ML_ATTCH.elementAt(index);
        for (int n = 0;n < attachments.length;n++) {
            line1   = EMOJI_ATTACHMENT + " " + attachments[n];
            linelen = line1.length();
            lines.  addElement(line1);
            linepos.addElement(new Integer(offset));
            CURRENT_MAIL_ANCHORS_TYPE. addElement(new Integer(ANCHOR_TYPE_ATTACH));
            CURRENT_MAIL_ANCHORS_TAG.  addElement(new Integer(n));
            CURRENT_MAIL_ANCHORS_RANGE.addElement(new int[] {offset,offset+linelen});
            offset += linelen;
        }

        //  自動折り返しおよび改行コードでメール本文を行ごとに分解する
        int screenWidth = _canvas.getWidth();
        int bodyLength = body.length();
        int lineStart = 0;
        while(0 < bodyLength) {
            int    breakLine;
            ASSERT(body != null);
            breakLine = _font.getLineBreak(body,
                                          lineStart,
                                          bodyLength,
                                          screenWidth);
            ASSERT(0         <= lineStart);
            ASSERT(lineStart <  body.length());
            ASSERT(lineStart <  breakLine);
            ASSERT(breakLine <= body.length());

            line1 = body.substring(lineStart,breakLine);
            linepos.addElement(new Integer(lineStart + offset));

            int lineFeed = line1.indexOf("\n");
            if (lineFeed == -1) {
                bodyLength -=  breakLine - lineStart;
                lineStart   = breakLine;
                lines.addElement(line1);
                continue;
            }
            if (lineFeed == 0) {
                bodyLength -= 1;
                lineStart  += 1;
                lines.addElement("");
                continue;
            }
            lineFeed  += lineStart;
            ASSERT(lineStart < lineFeed);
            ASSERT(lineFeed  <= body.length());
            lineFeed++;
            bodyLength -= lineFeed - lineStart;
            lines.addElement(body.substring(lineStart,lineFeed));
            lineStart   = lineFeed;
        }

        parseMailBody(body,offset);
        offset += body.length();

        //  ページアンカー追加
        CURRENT_MAIL_PAGE_COUNT = 1;
        if(CURRENT_FOLDER <= FLD_READ) {
            ASSERT(ML_PGCOUNT.elementAt(index) != null);
            CURRENT_MAIL_PAGE_COUNT = 
                ((Integer)ML_PGCOUNT.elementAt(index)).intValue();
        }

        //  次のページ [1]
        line1   = GETMSTR(IDS_MAIL_BODY_NEXT_PAGE);
        linelen = line1.length();
        lines.  addElement(line1);
        linepos.addElement(new Integer(offset));
        if (currentpage < CURRENT_MAIL_PAGE_COUNT) {
            CURRENT_MAIL_ANCHORS_TYPE. addElement(new Integer(ANCHOR_TYPE_PAGE));
            CURRENT_MAIL_ANCHORS_TAG.  addElement(new Integer(currentpage + 1));
            CURRENT_MAIL_ANCHORS_RANGE.addElement(new int[]{offset,offset+linelen});
        }
        offset += linelen;
        //  前のページ [2]
        line1   = GETMSTR(IDS_MAIL_BODY_PREV_PAGE);
        linelen = line1.length();
        lines.  addElement(line1);
        linepos.addElement(new Integer(offset));
        if (1 < currentpage) {
            CURRENT_MAIL_ANCHORS_TYPE. addElement(new Integer(ANCHOR_TYPE_PAGE));
            CURRENT_MAIL_ANCHORS_TAG.  addElement(new Integer(currentpage - 1));
            CURRENT_MAIL_ANCHORS_RANGE.addElement(new int[]{offset,offset+linelen});
        }
        offset += linelen;

        //  各ページのアンカー
        for (int page = 1;page <= CURRENT_MAIL_PAGE_COUNT;page++) {
            line1   = EMOJI_PAGE + " " + page + "ページ";
            linelen = line1.length();
            lines.  addElement(line1);
            linepos.addElement(new Integer(offset));
            if (currentpage  != page) {
                CURRENT_MAIL_ANCHORS_TYPE. addElement(new Integer(ANCHOR_TYPE_PAGE));
                CURRENT_MAIL_ANCHORS_TAG.  addElement(new Integer(page));
                CURRENT_MAIL_ANCHORS_RANGE.addElement(new int[]{offset,offset+linelen});
            }
            offset += linelen;
        }
        CURRENT_MAIL_FORMATED = lines;
        CURRENT_MAIL_LINEPOS  = linepos;

        //  アクティブアンカーを求める
        ACTIVE_ANCHOR = 0;
        if (CURRENT_MAIL_ANCHORS_TYPE.size() == 0) {
            ACTIVE_ANCHOR = -1;
        }

        ASSERT(CURRENT_MAIL_ANCHORS_TYPE.size() == CURRENT_MAIL_ANCHORS_TAG.size());
        ASSERT(CURRENT_MAIL_ANCHORS_TYPE.size() == CURRENT_MAIL_ANCHORS_RANGE.size());
        ASSERT(CURRENT_MAIL_LINEPOS.size() == 0 && ACTIVE_ANCHOR == -1 ||
               CURRENT_MAIL_LINEPOS.size() != 0);
        
        _mailCurrentPageNo = currentpage;
        openMailBody();
    }

    private static void openMailBody() {
        _currentScene          = SCENE_MAIL_BODY;
        _mailBodyRedrawBody = true;
        startTimer();
        activateCanvas(GETMSTR(IDS_SFK_RETURN),GETMSTR(IDS_SFK_MENU));
    }

    /**
     *  指定した順番のメール本文閲覧画面を開く.
     *  @param index 現在のメールフォルダ内のインデックス
     */
    private static boolean openMailBody(int index) {
        int current = MAIL_INDEX_LIST[index];
        if(CURRENT_FOLDER == FLD_NEW) {
            if(!protocolGetBody(current,1)) {
                return false;
            }
            ML_FOLDERID.setElementAt(new Integer(FLD_READ),current);
        }

        String page = (String)ML_BODY.elementAt(current);
        ASSERT(page != null);
        _mailListState[LIST_STATE_SELECT] = CURRENT_MAIL_INDEX = index;
        CURRENT_MAIL_PAGE_COUNT = 0;
        DOWNLOADED_BODY_PAGE    = new Vector();
        DOWNLOADED_BODY_PAGENO  = new Vector();
        DOWNLOADED_BODY_PAGE  .addElement(page);
        DOWNLOADED_BODY_PAGENO.addElement(new Integer(1));
        openMailBody(current,page,1);
        return true;
    }


    /**
     *  次/前のメールを閲覧する
     *  ただし存在しない場合はフォルダに戻る.
     */
    private static void nextMailBody(int offset) {
        ASSERT(offset == -1 || offset == 1);

        int count = MAIL_INDEX_LIST.length;
        int newIndex= CURRENT_MAIL_INDEX + offset;
        TRACE("newIndex=" + newIndex);
        if (newIndex < 0 || count <= newIndex) {
            return2MailList();
            return;
        }

        if (!openMailBody(newIndex)) {
            showDialog(GETMSTR(IDS_CAN_NOT_DOWNLOAD_BODY),Dialog.DIALOG_ERROR);
        }
        _mailBodyRedrawBody = true;
        startTimer();
    }

    /**
     *  メール本文閲覧画面を描画する
     *
     *  同期をとり二重に描画しないようにする
     */
    private static void paintMailBody(Graphics g) {
        Image image    = MRIMAGES[MIMAGE_MAIL];
        int screenHeight   = _canvas.getHeight();
        int screenWidth    = _canvas.getWidth();
        int fontHeight     = _font.getHeight();
        int fontAscent     = _font.getAscent();
        int titleHeight    = fontHeight + 4;
        int headLineHeight = Math.max(fontHeight + 4,IMAGE_MAIL_ICON_HEIGHT);
        int headHeight     = headLineHeight * 3;
        int scrollheight   = _mailBodyScroll * fontHeight;

        //  メール本文の描画
        if (_mailBodyRedrawBody) {
            _mailBodyRedrawBody = false;

            //  本文背景を描画
            setColorOfRGB(g,_colors[COLOR_CLIENT_BKGND]);
            g.fillRect(0,0,screenWidth,screenHeight);

            int y = titleHeight + Math.max(headHeight - scrollheight,0);
            int lineno = Math.max(_mailBodyScroll - (headHeight + titleHeight) / fontHeight,0);
            int anchorCount = CURRENT_MAIL_ANCHORS_RANGE.size();
            int currentAnchor = -1;
            if (0 < anchorCount) {
                currentAnchor = 0;
                int lineStartPos = ((Integer)CURRENT_MAIL_LINEPOS.elementAt(lineno)).intValue();
                while (currentAnchor < anchorCount) {
                    int[] range = (int[])CURRENT_MAIL_ANCHORS_RANGE.elementAt(currentAnchor);
                    if (lineStartPos < range[1]) {
                        break;
                    }
                    currentAnchor++;
                }
            }
            //  各行ループ
            for(;;) {
                if (CURRENT_MAIL_FORMATED.size() <= lineno) {
                    break;
                }
                if (screenHeight < y) {
                    break;
                }
                currentAnchor  = drawMailBodyLine(g,currentAnchor,lineno,y);
                y             += fontHeight;
                lineno++;
            }
            TRACE("paint-end-line:" + lineno);
        }

        // 宛先 / 日付 を描画する (+ ヘッダ領域の消去)
        int y = titleHeight - scrollheight;
        drawBox(g,0,y,screenWidth,headHeight,_colors[COLOR_BKGND],_colors[COLOR_FRAME]);

        int x = IMAGE_MAIL_ICON_WIDTH + ICON_TEXT_MARGIN;
        int w = screenWidth - x;

        // 差出人 / 宛先名の描画
        int iconLeft;
        if(CURRENT_FOLDER == FLD_SEND || CURRENT_FOLDER == FLD_DRAFT) {
            iconLeft = IMAGE_MAIL_ICON_WIDTH * IMAGE_MAIL_ICON_INDEX_TO;
        } else {
            iconLeft = IMAGE_MAIL_ICON_WIDTH * IMAGE_MAIL_ICON_INDEX_FROM;
        }
        g.setColor(_colors[COLOR_TEXT]);
        drawText(g,CURRENT_MAIL_MAIL,x,y,w,headLineHeight,2,false,ALIGN_LEFT);
        g.drawImage (image,0,y,iconLeft,0,IMAGE_MAIL_ICON_WIDTH,IMAGE_MAIL_ICON_HEIGHT);
        y += headLineHeight;

        // メール日付の描画
        iconLeft = IMAGE_MAIL_ICON_INDEX_DATE * IMAGE_MAIL_ICON_WIDTH;
        drawText(g,CURRENT_MAIL_DATE,x,y,w,headLineHeight,2,false,ALIGN_LEFT);
        g.drawImage (image,0,y,iconLeft,0,IMAGE_MAIL_ICON_WIDTH,IMAGE_MAIL_ICON_HEIGHT);
        y += headLineHeight;

        //  件名を表示する
        drawTicker(g,CURRENT_MAIL_SUBJECT,getTicker(),x,y,w,headLineHeight,2);

        g.drawImage (image,0,y,iconLeft,0,IMAGE_MAIL_ICON_WIDTH,IMAGE_MAIL_ICON_HEIGHT);

        //  タイトルを描画
        String title = GETMSTR(IDS_MAILBOX_GET);
        if(CURRENT_FOLDER==FLD_READ) {
            title = GETMSTR(IDS_MAILBOX_READ);
        }
        if(CURRENT_FOLDER==FLD_SEND) {
            title = GETMSTR(IDS_MAILBOX_SEND);
        }
        if(CURRENT_FOLDER==FLD_DRAFT) {
            title = GETMSTR(IDS_MAILBOX_DRAFT);
        }
        if(CURRENT_FOLDER==FLD_SAVE) {
            title = GETMSTR(IDS_MAILBOX_SAVE);
        }
        title += GETMSTR(IDS_MAIL);
        paintTitle(g,title);

    }

    /**
    *   メール本文の行を描画する
    *
    *   @param currentAnchor 描画中カレントアンカー
    *   @param lineno        描画する行
    *   @param y             表示する行の描画先Y座標
    */
     private static int drawMailBodyLine(Graphics g,int currentAnchor,int lineno,int y) {
        ASSERT(CURRENT_MAIL_FORMATED != null);
        ASSERT(CURRENT_MAIL_LINEPOS  != null);
        ASSERT(CURRENT_MAIL_FORMATED.size() == CURRENT_MAIL_LINEPOS.size());
        ASSERT(CURRENT_MAIL_FORMATED.size() != 0);
        ASSERT(0 <= lineno && lineno < CURRENT_MAIL_FORMATED.size());
        ASSERT(CURRENT_MAIL_ANCHORS_TYPE != null);
        ASSERT(CURRENT_MAIL_ANCHORS_TYPE.size() == CURRENT_MAIL_ANCHORS_TAG.size());
        ASSERT(CURRENT_MAIL_ANCHORS_TYPE.size() == CURRENT_MAIL_ANCHORS_RANGE.size());
        ASSERT(_font != null);

        int fontHeight  = _font.getHeight();
        int fontAscent  = _font.getAscent();
        String line     = (String)  CURRENT_MAIL_FORMATED. elementAt(lineno);
        int lineStart   = ((Integer)CURRENT_MAIL_LINEPOS.elementAt(lineno)).intValue();
        int lineEnd     = lineStart + line.length();
        int x           = 0;
        int drawStart   = lineStart;
        int anchorCount = CURRENT_MAIL_ANCHORS_RANGE.size();
        if (lineStart == lineEnd) {
            return currentAnchor;
        }
        ASSERT(drawStart < lineEnd);
        while (drawStart < lineEnd) {
            int drawEnd;
            if (currentAnchor == anchorCount) {
                currentAnchor = -1;
            }
            if (currentAnchor != -1) {
                int[] range;
                range = (int[])CURRENT_MAIL_ANCHORS_RANGE.elementAt(currentAnchor);
                ASSERT(range != null);
                ASSERT(range.length == 2);
                //TRACE("search " + currentAnchor + ":" + range[1] + "<=" + lineStart);
                if (range[1] <= drawStart) {
                    //TRACE("next");
                    currentAnchor++;
                }
            }
            if (currentAnchor == anchorCount) {
                currentAnchor = -1;
            }

            //  現在アクティブなアンカーはドキュメントアンカー
            boolean inAnchor = false;
            boolean inActiveAnchor = false;
            int activeAnchorType = ANCHOR_TYPE_INACTIVE;

            //  それ以外
            drawEnd = lineEnd;
            if (currentAnchor != -1) {
                ASSERT(currentAnchor < CURRENT_MAIL_ANCHORS_TYPE.size());
                int[] range;
                activeAnchorType 
                      = ((Integer)CURRENT_MAIL_ANCHORS_TYPE. elementAt(currentAnchor)).intValue();
                range = (int[])   CURRENT_MAIL_ANCHORS_RANGE.elementAt(currentAnchor);
                ASSERT(range != null);
                ASSERT(range.length == 2);
                //TRACE("range[0]=" + range[0]);
                //TRACE("range[1]=" + range[1]);
                // 現在アンカー内
                if (range[0] <= drawStart) {
                    inAnchor       = true;
                    inActiveAnchor = currentAnchor == ACTIVE_ANCHOR;
                    drawEnd = range[1];
                    //  アンカーはこの行で終わる
                    if (lineEnd < range[1]) {
                        //  アンカーは次の行まで続く
                        drawEnd = lineEnd;
                    } 
                // 現在アンカー外
                } else {
                    // カレントアンカーはこの行から開始
                    drawEnd = range[0];
                    // カレントアンカーは次の行から開始
                    if (lineEnd <= range[0]) {
                        drawEnd = lineEnd;
                    }
                }
            } 

            //  描画する
            ASSERT(line != null);
            ASSERT(drawStart < drawEnd);
            ASSERT(lineStart <= drawStart);
            ASSERT(drawEnd   <= lineEnd);
            String draw      = line.substring(drawStart - lineStart,drawEnd - lineStart);
            int drawWidth = _font.stringWidth(draw);
            if (inActiveAnchor) {
                setColorOfRGB(g,_colors[COLOR_SELECT_BKGND]);
                if (activeAnchorType == ANCHOR_TYPE_PAGE ||
                    activeAnchorType == ANCHOR_TYPE_ATTACH) {
                    g.fillRect(0,y,_canvas.getWidth(),fontHeight);
                } else {
                    g.fillRect(x,y,drawWidth,fontHeight);
                }
                setColorOfRGB(g,_colors[COLOR_SELECT_TEXT]);
            } else if (inAnchor) {
                setColorOfRGB(g,_colors[COLOR_ANCHOR_TEXT]);
            } else {
                setColorOfRGB(g,_colors[COLOR_TEXT]);
            }
            g.drawString(draw,x,y + fontAscent);

            //  次のパートへ
            x        += drawWidth;
            drawStart = drawEnd;
        }
        return currentAnchor;
     }

    /**
    *   anchorIndexで指定したアンカーが画面上で可視かどうか
    *   @param anchorIndex アンカーインデックス
    *   @return true  - 可視
    *           false - 不可視
    */
    private static boolean isAnchorVisible(int anchorIndex) {
        ASSERT(0 <= anchorIndex && anchorIndex < CURRENT_MAIL_ANCHORS_RANGE.size());
        ASSERT(CURRENT_MAIL_LINEPOS.size() != 0);

        int fontHeight = _font.getHeight();
        int titleHeight = fontHeight + 4;
        int headLineHeight = Math.max(IMAGE_MAIL_ICON_HEIGHT,fontHeight + 4);
        int headHeight = headLineHeight * 3;
        int scrollheight = _mailBodyScroll * fontHeight;
        int visibleLine = (_canvas.getHeight() - (headHeight + titleHeight)) / fontHeight - 1;

        //  可視範囲の行を求める
        int startline = _mailBodyScroll;
        int endline = Math.min(_mailBodyScroll + visibleLine,CURRENT_MAIL_LINEPOS.size() - 1);
        TRACE("line: " + startline + "-" + endline + "/" + CURRENT_MAIL_LINEPOS.size());
        ASSERT(0 <= startline && startline < CURRENT_MAIL_LINEPOS.size());
        ASSERT(0 <= startline && endline   < CURRENT_MAIL_LINEPOS.size());
        ASSERT(startline <= endline);

        //  可視範囲の文字位置を求める
        int startpos = ((Integer)CURRENT_MAIL_LINEPOS.elementAt(startline)).intValue();
        int endpos   = ((Integer)CURRENT_MAIL_LINEPOS.elementAt(endline)).intValue()
                     + ((String) CURRENT_MAIL_FORMATED.elementAt(endline)).length();
        int[] range  = (int[])CURRENT_MAIL_ANCHORS_RANGE.elementAt(anchorIndex);
        TRACE("pos-range:" + startpos + "-" + endpos);
        TRACE("range:" + range[0] + "-" + range[1]);
        return !(range[1] <= startpos || endpos <= range[0]);
    }

    /** アンカーを検索する
    *   アクティブアンカーがない場合→キーにしたがってスクロール
    *   アクティブアンカーがある場合
    *       アンカーが不可視
    *           キーにしたがってスクロール
    *           次のアンカーは可視？
    *               次のアンカーへ移動して
    *       アンカーが可視
    *           次のアンカーは可視
    *               次のアンカーへ移動して
    *               スクロールはしない
    *           次のアンカーが不可視
    *               スクロールする
    *   @param  direct  MAIL_SCROOL_DIRECT_UP   
    *                   MAIL_SCROOL_DIRECT_DOWN 
    *   @return MAIL_SCROOL_DIRECT_NONE
    *           MAIL_SCROOL_DIRECT_UP
    *           MAIL_SCROOL_DIRECT_DOWN
    */
    private static int searchAnchor(int direct) {
        ASSERT(direct == MAIL_SCROOL_DIRECT_UP ||
               direct == MAIL_SCROOL_DIRECT_DOWN);
        ASSERT(CURRENT_MAIL_ANCHORS_TYPE  != null);
        ASSERT(CURRENT_MAIL_ANCHORS_TAG   != null);
        ASSERT(CURRENT_MAIL_ANCHORS_RANGE != null);

        int anchorCount = CURRENT_MAIL_ANCHORS_TYPE.size();
        ASSERT(CURRENT_MAIL_ANCHORS_TAG.size()   == anchorCount);
        ASSERT(CURRENT_MAIL_ANCHORS_RANGE.size() == anchorCount);
        if (ACTIVE_ANCHOR == -1) {
            ASSERT(0 == anchorCount);
           return direct;
        }
        ASSERT(0 < anchorCount);

        int nextAnchor = ACTIVE_ANCHOR + direct;
        nextAnchor = Math.max(nextAnchor,0);
        nextAnchor = Math.min(nextAnchor,anchorCount - 1);

        if (nextAnchor != ACTIVE_ANCHOR && isAnchorVisible(nextAnchor)) {
            if (isAnchorVisible(ACTIVE_ANCHOR)) {
                direct = MAIL_SCROOL_DIRECT_NONE;
            }
            ACTIVE_ANCHOR = nextAnchor;
        }
        return direct;
    }

    /**
    *   メール本文画面でのキー操作
    *   @see Canvas.processEvent
    */
    private static void processMailBody(int key) {
        int mailIndex = MAIL_INDEX_LIST[CURRENT_MAIL_INDEX];

        if (key == Display.KEY_1 && _mailCurrentPageNo < CURRENT_MAIL_PAGE_COUNT) {
            nextPage(_mailCurrentPageNo+1);
        }
        if (key == Display.KEY_2 && 1 < _mailCurrentPageNo) {
            nextPage(_mailCurrentPageNo-1);
        }

        int direct = MAIL_SCROOL_DIRECT_NONE;
        int scroll = 0;
        if (key == Display.KEY_ASTERISK) {
            scroll = -6;
            direct = MAIL_SCROOL_DIRECT_UP;
        }
        if (key == Display.KEY_POUND)    {
            scroll =  6;
            direct = MAIL_SCROOL_DIRECT_DOWN;
        }
        if (key == Display.KEY_UP)       {
            scroll = -3;
            direct = MAIL_SCROOL_DIRECT_UP;
        }
        if (key == Display.KEY_DOWN)     {
            scroll =  3;
            direct = MAIL_SCROOL_DIRECT_DOWN;
        }

        if (direct == MAIL_SCROOL_DIRECT_UP ||
            direct == MAIL_SCROOL_DIRECT_DOWN) {
            direct = searchAnchor(direct);
            _mailBodyRedrawBody = true;
        }

        if (direct != MAIL_SCROOL_DIRECT_NONE) {
            _mailBodyScroll += scroll;
            int visibleLine = (_canvas.getHeight() - (_font.getHeight() + 4) * 4) / _font.getHeight() - 1;
            TRACE("_mailBodyScroll:" + _mailBodyScroll + "/" + (CURRENT_MAIL_LINEPOS.size() - visibleLine));
            _mailBodyScroll = Math.min(_mailBodyScroll,CURRENT_MAIL_LINEPOS.size() - visibleLine);
            _mailBodyScroll = Math.max(_mailBodyScroll,0);
            _mailBodyRedrawBody = true;
            _canvas.repaint2();
            repeatKey();
            return;
        }

        if (key == Display.KEY_SOFT1) {
            waitKeyRelease();
            return2MailList();
            return;
        }

        if (key == Display.KEY_SOFT2) {
            int   idm;
            int[] mapping;
            switch(CURRENT_FOLDER) {
            case FLD_SEND:
                idm = IDM_MAIL_BODY_SEND;
                mapping = new int[] {4,5,6};
                break;
            case FLD_DRAFT:
                idm = IDM_MAIL_BODY_DRAFT;
                mapping = new int[] {4,5,6};
                break;
            case FLD_SAVE:
                idm = IDM_MAIL_BODY_SAVE;
                mapping = new int[] {1,2,3,5,6};
                break;
            default:
                idm = IDM_MAIL_BODY_NEW;
                mapping = new int[] {0,1,2,3,5,6};
                break;
            }
            int select = trackMenu(null,loadStrings(MRESOURCES[idm]),0,true);
            if (select != -1) {
                select = mapping[select];
                menuMailBody(select);
                startTimer();
            }
            _mailBodyRedrawBody = true;
            repaint2();
            waitKeyRelease();
            return;
        }

        if (key == Display.KEY_LEFT) {
            nextMailBody(-1);
            waitKeyRelease();
            return;
        }

        if (key == Display.KEY_RIGHT) {
            nextMailBody(1);
            repeatKey();
            return;
        }

//-----------------------------------------------------------------------------
//  for PointingDevice.
//-----------------------------------------------------------------------------
#if defined(USE_POINTING_DEVICE)
        if (key == EVENT_POINTER_SELECTED) {
            int titleHeight    = _font.getHeight() + 4;
            int headLineHeight = Math.max(IMAGE_MAIL_ICON_HEIGHT,_font.getHeight() + 4);
            int headHeight     = titleHeight + headLineHeight * 3;
            int line = POINTING_DEVICE_GET_Y() - headHeight;
            if (0 <= line) {
                line = line / _font.getHeight() + _mailBodyScroll;
                if (line < CURRENT_MAIL_LINEPOS.size()) {
                    int lineStartPos = ((Integer)CURRENT_MAIL_LINEPOS.elementAt(line)).intValue();
                    int clickPos     = lineStartPos + POINTING_DEVICE_GET_X() / _font.stringWidth("w");
                    int anchorCount  = CURRENT_MAIL_ANCHORS_RANGE.size();
                    int clickAnchor  = -1;
                    for (int n = 0;n < anchorCount;n++) {
                        int[] range = (int[])CURRENT_MAIL_ANCHORS_RANGE.elementAt(n);
                        if (range[0] <= clickPos && clickPos < range[1]) {
                            clickAnchor = n;
                            break;
                        }
                    }
                    if (clickAnchor != -1) {
                        if (ACTIVE_ANCHOR == clickAnchor) {
                            key = Display.KEY_SELECT;
                        } else {
                            ACTIVE_ANCHOR       = clickAnchor;
                            _mailBodyRedrawBody = true;
                            _canvas.repaint2();
                            return;
                        }
                    }
                }
            }
        }
#endif
//-----------------------------------------------------------------------------
        if (key != Display.KEY_SELECT) {
            repeatKey();
            _mailBodyRedrawBody = true;
            _canvas.repaint2();
            return;
        }

        //  選択キーを押下
        int activeAnchorIndex = ACTIVE_ANCHOR;
        if (activeAnchorIndex == -1) {
            return;
        }
        if (!isAnchorVisible(activeAnchorIndex)) {
            return;
        }
        waitKeyRelease();

        ASSERT(CURRENT_MAIL_ANCHORS_TYPE != null);
        ASSERT(0 <= activeAnchorIndex && 
               activeAnchorIndex < CURRENT_MAIL_ANCHORS_TYPE.size());
        ASSERT(CURRENT_MAIL_ANCHORS_TAG  != null);
        ASSERT(CURRENT_MAIL_ANCHORS_TAG.size() ==
               CURRENT_MAIL_ANCHORS_TYPE.size());
        int activeAnchorType = ((Integer)CURRENT_MAIL_ANCHORS_TYPE.elementAt(activeAnchorIndex)).intValue();
        Object anchorTag     = CURRENT_MAIL_ANCHORS_TAG.elementAt(activeAnchorIndex);

        //  添付ファイル
        if (activeAnchorType == ANCHOR_TYPE_ATTACH) {
            int attachIndex = ((Integer)anchorTag).intValue();
            String fname = ((String[])ML_ATTCH.elementAt(mailIndex))[attachIndex];
            fname = fname.toLowerCase();
            int mediaType = -1;
            if (fname.endsWith(".gif") ||
                fname.endsWith(".jpg")) {
                mediaType = MEDIA_TYPE_IMAGE;
            }
            /*
            if (fname.endsWith(".mld")) {
                mediaType = MEDIA_TYPE_SOUND;
            }
            */
            if (mediaType == -1) {
                showDialog(GETMSTR(IDS_UNSUPPORTED_ATTACHMENT),
                           Dialog.DIALOG_ERROR);
                startTimer();
                _mailBodyRedrawBody = true;
                return;
            }

            if(!protocolGetAttachment(mailIndex,attachIndex))  {
                showDialog(GETMSTR(IDS_CANNOT_DOWNLOAD_ATTACHMENT),
                           Dialog.DIALOG_ERROR);
                startTimer();
                _mailBodyRedrawBody = true;
                return;
            }

            if(mediaType == MEDIA_TYPE_IMAGE) {
                openImgPlayer(FILEID_INVALID,CURRENT_ATTACH);
                return;
            }
            
            if(mediaType == MEDIA_TYPE_SOUND) {
                openSndPlayer(FILEID_INVALID,CURRENT_ATTACH);
                return;
            }
            TRACE("mediaType=" + mediaType);
            ASSERT(false);
            return;
        }

        if (activeAnchorType== ANCHOR_TYPE_URL) {
            if(showDialog(GETMSTR(IDS_URL_ACCESS_CONFIRM),Dialog.DIALOG_YESNO)
                 != Dialog.BUTTON_YES) {
                _mailBodyRedrawBody = true;
                return;
            }
            openBrowser(BROWSER_MODE_STANDARD,(String)anchorTag);
            return;
        }

        if (activeAnchorType == ANCHOR_TYPE_MAILTO) {
            // CURRENT_FOLDER = FLD_CREATE;
            openMailEdit(SCENE_MAIL_BODY,
                         (String)anchorTag,
                         "",
                         "",
                         FILEID_INVALID);
            return;
        }

        if (activeAnchorType == ANCHOR_TYPE_TELTO) {
            try {
                Phone.call((String)anchorTag);
            } catch(Throwable e) {
                DEBUG_ONLY(e.printStackTrace());
            }
            startTimer();
            _mailBodyRedrawBody = true;
            return;
        }

        ASSERT(activeAnchorType== ANCHOR_TYPE_PAGE);
        int pageNo = ((Integer)anchorTag).intValue();
        ASSERT(0 <= pageNo);
        nextPage(pageNo);
    }

    /**
     *
     */
    private static final void nextPage(int pageNo) {
        int mailIndex = MAIL_INDEX_LIST[CURRENT_MAIL_INDEX];
        int cache = DOWNLOADED_BODY_PAGENO.indexOf(new Integer(pageNo));
        ASSERT(cache == -1 || 0 <= cache && cache < CURRENT_MAIL_PAGE_COUNT);
        if (cache != -1) {
            openMailBody(mailIndex,(String)DOWNLOADED_BODY_PAGE.elementAt(cache),pageNo);
            return;
        }

        if(!protocolGetBody(mailIndex,pageNo)) {
            showDialog(GETMSTR(IDS_CAN_NOT_DOWNLOAD_BODY),Dialog.DIALOG_ERROR);
            _mailBodyRedrawBody = true;
            return;
        }

        Object newCache = ML_BODY.elementAt(mailIndex);
        DOWNLOADED_BODY_PAGENO.addElement(new Integer(pageNo));
        DOWNLOADED_BODY_PAGE.  addElement(newCache);
        ML_BODY.setElementAt(DOWNLOADED_BODY_PAGE.elementAt(0),
                             mailIndex);
        openMailBody(mailIndex,(String)newCache,pageNo);
    }

    /**
    *   メール本文のポップアップメニューのキーイベント処理
    */
    private static void menuMailBody(int select) {
        String mailto;
        String mailsubject;
        String mailbody;
        int mailIndex = MAIL_INDEX_LIST[CURRENT_MAIL_INDEX];
        if (select == 0) {
            //  セーブするかどうか確認する
            if(showDialog(GETMSTR(IDS_MAIL_SAVE_CONFIRM),Dialog.DIALOG_YESNO)
                !=Dialog.BUTTON_YES) {
                _mailBodyRedrawBody = true;
                startTimer();
                _canvas.repaint2();
                return;
            }
            _mailBodyRedrawBody = true; 
            _canvas.repaint2();

            ML_MAIL.     addElement(ML_MAIL.elementAt(mailIndex));
            ML_SUBJECT.  addElement(ML_SUBJECT.elementAt(mailIndex));
            ML_IUIDL.    addElement(ML_IUIDL.elementAt(mailIndex));
            ML_DATE.     addElement(ML_DATE.elementAt(mailIndex));
            ML_BODY.     addElement(CURRENT_MAIL_BODY);
            ML_ATTCH.    addElement(ML_ATTCH.elementAt(mailIndex));
            ML_PGCOUNT.  addElement(ML_PGCOUNT.elementAt(mailIndex));
            ML_ACCOUNTID.addElement(ML_ACCOUNTID.elementAt(mailIndex));
            ML_FOLDERID. addElement(new Integer(FLD_SAVE));
            ML_SATTACH  .addElement(new Integer(-1));

            if (saveMail()) {
                showDialog(GETMSTR(IDS_MAIL_SAVE_DONE),Dialog.DIALOG_INFO);
            }
            return;
        }
        //  メールに返信する
        if (select == 1) {
            mailto      = CURRENT_MAIL_MAIL;
            mailsubject = "Re:"+CURRENT_MAIL_SUBJECT.trim();
            mailbody    = "";
            openMailEdit(SCENE_MAIL_LIST,
                         mailto,
                         mailsubject,
                         mailbody,
                         FILEID_INVALID);
            return;
        }
        //  メールに転送する
        if(select == 2) {
            mailto      = "";
            mailsubject = "Fwd:"+CURRENT_MAIL_SUBJECT;
            mailbody    = "\n\n\n"
                              + GETMSTR(IDS_FRWD)
                              + "\n"
                              + "CURRENT_MAIL_FROM: "
                              + CURRENT_MAIL_MAIL
                              + "\nSubject: "
                              + CURRENT_MAIL_SUBJECT
                              + "\nBody: "
                              + CURRENT_MAIL_BODY;
            openMailEdit(SCENE_MAIL_LIST,
                         mailto,
                         mailsubject,
                         mailbody,
                         FILEID_INVALID);
            return;
        }
        //  メールを引用返信
        if(select == 3) {
            mailto      = CURRENT_MAIL_MAIL;
            mailsubject = "Re:"+CURRENT_MAIL_SUBJECT;
            mailbody    = "\n\n\n---- "
                              + CURRENT_MAIL_MAIL
                              + " wrote:\n"
                              + CURRENT_MAIL_BODY;
            openMailEdit(SCENE_MAIL_LIST,
                         mailto,
                         mailsubject,
                         mailbody,
                         FILEID_INVALID);
            return;
        }
        //  メールを編集送信
        if(select == 4) {
            openMailEdit(SCENE_MAIL_LIST,
                         CURRENT_MAIL_MAIL,
                         CURRENT_MAIL_SUBJECT,
                         CURRENT_MAIL_BODY,
                         FILEID_INVALID);
            return;
        }
        //  削除
        if (select == 5) {
            if(showDialog(GETMSTR(IDS_DEL_CONFIRM),Dialog.DIALOG_YESNO) != Dialog.BUTTON_YES) {
                startTimer();
                repaint2();
                return;
            }

            DEBUG_ONLY(validateML());
            removeML(mailIndex);
            DEBUG_ONLY(validateML());
            if(FLD_READ < CURRENT_FOLDER) {
                saveMail();
            }
            
            showDialog(GETMSTR(IDS_DEL_DONE),Dialog.DIALOG_INFO);
            openFolderList();
            return;
        }
        //  アドレス帳に登録
        if (select == 6) {
            String mail;
            String name;
            int beg;
            int end;
            
            mail = CURRENT_MAIL_MAIL;
            name = "";
            beg = mail.indexOf("<");
            end = mail.indexOf(">");
            if (beg != -1) {
                name = mail.substring(0,beg);
                mail = mail.substring(beg+1,end);
            }
            openAddressEdit(SCENE_MAIL_BODY,
                            new String[] {mail,name,"",""},
                            -1);
            return;
        }
    }

    /**
      アンカーを検索する
      単語として許されている文字のみを含む単語を切り出す
          [単語/状態]  正規文字     非正規文字            終端
          正規文字     単語追加     単語終端              単語終端
                                    非正規文字状態に遷移  終了
          非正規文字   単語開始     読み飛ばし            終了
          初期状態     単語開始     読み飛ばし            終了
    */
    private static void parseMailBody(String body,int offset) {
        int    start;
        int    state;
        int    search;
        int    bodyLength;
        String word;
        word   = null;
        search  = 0;
        start  = 0;
        state  = WORDING_STATE_INV;
        bodyLength = body.length();
        for(;;) {
            char    c;
            boolean regular;
            int     action;

            c = 0;
            if (search == bodyLength) {
                action = WORDING_ACT_END;
            } else {
                c = body.charAt(search);
                action = WORDING_ACT_INV;
                if (WORD_CHAR.indexOf(c) != -1) {
                    action = WORDING_ACT_REG;
                }
            }

            if (state == WORDING_STATE_REG) {
                if (action == WORDING_ACT_REG)  {
                    ASSERT(word != null);
                    word += c;
                } else {
                    ASSERT(action == WORDING_ACT_INV ||
                           action == WORDING_ACT_END);
                    int anchorType;
                    int anchorStart;
                    int anchorEnd;
                    anchorType  = ANCHOR_TYPE_INACTIVE;
                    anchorStart = start;
                    anchorEnd   = search;
                    ASSERT(word != null);
                    ASSERT(word.length() != 0);
                    int wordlength;
                    wordlength = word.length();
                    //  word
                    if (word.startsWith("http://") ||
                        word.startsWith("https://")) {
                        if (c == ',') {
                            ASSERT(action == WORDING_ACT_INV);
                            word += c;
                            search++;
                            continue;
                        }
                        anchorType  = ANCHOR_TYPE_URL;
                    }
                    if (7 <= wordlength        &&
                        0 <  word.indexOf('@')) {
                        if (word.startsWith("mailto:")) {
                            word = word.substring(7);
                        }
                        anchorType  = ANCHOR_TYPE_MAILTO;
                        anchorStart = start;
                    }
                    if (9 <= wordlength &&
                        wordlength <= 14 &&
                        checkString(CHECK_STRING_TELE,word)) {
                        anchorType  = ANCHOR_TYPE_TELTO;
                    }

                    if (anchorType != ANCHOR_TYPE_INACTIVE) {
                        CURRENT_MAIL_ANCHORS_TYPE. addElement(new Integer(anchorType));
                        CURRENT_MAIL_ANCHORS_TAG.  addElement(word);
                        CURRENT_MAIL_ANCHORS_RANGE.addElement(
                            new int[]{offset+anchorStart,offset+anchorEnd});
                    }
                    word  = null;
                    state = WORDING_STATE_INV;
                }
                if (action == WORDING_ACT_END) {
                    break;
                }
            }
            if (state == WORDING_STATE_INV) {
                if (action == WORDING_ACT_REG)  {
                    start = search;
                    word  = "" + c;
                    state = WORDING_STATE_REG;
                }
                if (action == WORDING_ACT_END) {
                    break;
                }
            }
            search++;
        }
    }
