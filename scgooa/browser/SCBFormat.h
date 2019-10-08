    /**
    *   HTMLドキュメントを整形する
    *   staticな_documentHtml変数に格納されているHTML-DOMをレイアウトして
    *   各ノードの位置決めを行う。
    */
    protected static void formatDocument() {
        if (_documentHtml == null) {
            return;
        }
        DOCUMENT_COLOR_BG   = Graphics.getColorOfName(Graphics.WHITE);
        DOCUMENT_COLOR_TEXT = Graphics.getColorOfName(Graphics.BLACK);
        DOCUMENT_COLOR_LINK = Graphics.getColorOfName(Graphics.NAVY);
        FORMATING_WIDTH_LINE    = 0;
        FORMATING_HEIGHT_LINE   = 0;
        FORMATING_Y             = 0;
        FORMATING_BREAK_CURRENT = newHtmlBreakLine(0,0);
        _documentHtml.breaks = new Vector();
        _documentHtml.breaks.addElement(FORMATING_BREAK_CURRENT);
        SCROLL                   = 0;
        ACTIVE_INTERACTIVE_INDEX = 0;
        FORMATING_ALIGN          = 
        PREV_FORMATING_ALIGN     = ATTRIBUTE_VALUE_ALIGN_LEFT;
        DELAY_BREAK              = false;
        formatNode(_documentHtml);
        DEBUG_ONLY(dumpDocumentBreaks());
        DOCUMENT_HEIGHT = FORMATING_Y + _font.getHeight();
        HtmlBreakLine_yNext(FORMATING_BREAK_CURRENT) = DOCUMENT_HEIGHT;
        System.gc();
    }


    /** HTMLドキュメントを整形する
        @param  node
        @param  alreadyInited 改行コードが既に初期化済みの場合
        @return true          このノードの次のノードの直前で改行する
    */
    private static boolean DELAY_BREAK;
    private static void formatNode(HtmlNode node) {
        final int widthClient = _canvas.getWidth();
        final int heightLine  = _font.getHeight();
        final int type = node.type;
        int saved = FORMATING_ALIGN;
        // node.y = FORMATING_Y;
        if (type != TAG_html) {
            node.breaks = null;
        }
        boolean broken = false;
        if (DELAY_BREAK) {
            if (FORMATING_WIDTH_LINE != 0) {
                FORMATING_ALIGN = PREV_FORMATING_ALIGN;
                breakBefore(node,0);
                broken          = true;
                FORMATING_ALIGN = saved;
            }
            DELAY_BREAK = false;
        }
        node.y = FORMATING_Y;

        TRACE_HTML_FORMAT("<" + DEBUGS.getTagName(type) + "> " + FORMATING_ALIGN);
        //
        //  HTMLテキストノードのレイアウト処理
        //
        if (type == TAG_text) {
            final String str = node.text;
            TRACE_HTML_FORMAT(str);
            if (str.length() == 0) {
                return;
            }
            final Font f = _font;

            int startIdx = 0;
            int len      = str.length();
            while (0 < len) {
                int endIdx;
                int breakWidth;
                String line;
                FORMATING_HEIGHT_LINE = Math.max(FORMATING_HEIGHT_LINE,heightLine);
                breakWidth            = widthClient - FORMATING_WIDTH_LINE;
                endIdx                = startIdx;
                if (0 < breakWidth) {
                    endIdx = f.getLineBreak(str, startIdx, len, widthClient - FORMATING_WIDTH_LINE);
                }
                line = str.substring(startIdx,endIdx);
                
                FORMATING_WIDTH_LINE += f.stringWidth(line);
                len     -= (endIdx - startIdx);
                startIdx =  endIdx;
                if (0 < len) {
                    breakBefore(node,endIdx);
                }
            }
            return;
        }
        
        /// HTMLタグのレイアウト処理
        if (type == TAG_title) {
            return;
        }
        if (type == TAG_PSEUDO_TERM) {
            return;
        }

        boolean foramtChild;
        boolean breakAfter;
        String  attr;
        foramtChild = true;
        breakAfter  = false;
        if (type == TAG_body) {
            DOCUMENT_COLOR_BG   = getAttributeColor(node,ATTRIBUTE_NAME_bgcolor,DOCUMENT_COLOR_BG);
            DOCUMENT_COLOR_TEXT = getAttributeColor(node,ATTRIBUTE_NAME_text,   DOCUMENT_COLOR_TEXT);
            DOCUMENT_COLOR_LINK = getAttributeColor(node,ATTRIBUTE_NAME_link,   DOCUMENT_COLOR_LINK);
            TRACE("DOCUMENT_COLOR_BG   = " + Integer.toHexString(DOCUMENT_COLOR_BG));
            TRACE("DOCUMENT_COLOR_TEXT = " + Integer.toHexString(DOCUMENT_COLOR_TEXT));
            TRACE("DOCUMENT_COLOR_LINK = " + Integer.toHexString(DOCUMENT_COLOR_LINK));
        }

        /// 画像(img)タグのレイアウト処理
        if (type == TAG_img) {
            int n = getAttributeInt(node,ATTRIBUTE_NAME_src,-1);
            MediaImage image;
            if (0 <= n && n < _inlineImages.size()) {
                image = (MediaImage)_inlineImages.elementAt(n);
            } else {
                image = null;
            }
            int w = getAttributePixelOrPercent(
                                      node,
                                      ATTRIBUTE_NAME_width,
                                      DEFAULT_IMAGE_SIZE);
            int h = getAttributePixelOrPercent(
                                      node,
                                      ATTRIBUTE_NAME_height,
                                      DEFAULT_IMAGE_SIZE);
            if (image != null) {
                w = image.getWidth();
                h = image.getHeight();
            }
            node.width  = w;
            node.height = h;

            
            if (FORMATING_WIDTH_LINE == 0 ||
                widthClient - FORMATING_WIDTH_LINE < w) {
                breakBefore(node,0);
                FORMATING_WIDTH_LINE  += w;
                FORMATING_HEIGHT_LINE = Math.max(FORMATING_HEIGHT_LINE,h);
            } else if (!broken){
                breakBefore(node,0);
                FORMATING_HEIGHT_LINE = h;
            } else {
                FORMATING_HEIGHT_LINE = Math.max(FORMATING_HEIGHT_LINE,h);
            }
        }

        /// 水平線(hr)のレイアウト処理
        if (type == TAG_hr) {
            if (!broken && 0 < FORMATING_WIDTH_LINE) {
                breakBefore(node,0);
            }
            ASSERT(FORMATING_WIDTH_LINE == 0);
            FORMATING_HEIGHT_LINE = heightLine;
            FORMATING_WIDTH_LINE  = widthClient;
            breakAfter            = true;
        }
        if (type == TAG_br) {
            FORMATING_HEIGHT_LINE = Math.max(FORMATING_HEIGHT_LINE,heightLine);
            if (!broken) {
                breakBefore(node,0);
            }
        }
        //  inputタグのフォーマット
        if (type == TAG_input) {
            int inputtype = getAttributeInt(node,ATTRIBUTE_NAME_type,ATTRIBUTE_VALUE_INPUT_TEXT);
            ASSERT(inputtype == ATTRIBUTE_VALUE_INPUT_TEXT     ||
                   inputtype == ATTRIBUTE_VALUE_INPUT_PASSWORD ||
                   inputtype == ATTRIBUTE_VALUE_INPUT_SUBMIT   ||
                   inputtype == ATTRIBUTE_VALUE_INPUT_RESET    ||
                   inputtype == ATTRIBUTE_VALUE_INPUT_CHECKBOX ||
                   inputtype == ATTRIBUTE_VALUE_INPUT_RADIO    ||
                   inputtype == ATTRIBUTE_VALUE_INPUT_HIDDEN);

            int w = 0;
            int h = 0;

            String value = getAttributeStr(node,ATTRIBUTE_NAME_value);
            if (value == null){
                value = "";
                if (inputtype == ATTRIBUTE_VALUE_INPUT_SUBMIT) {
                    value = GETMSTR(IDS_DEFAULT_SUBMIT);
                }
                if (inputtype == ATTRIBUTE_VALUE_INPUT_RESET) {
                    value = GETMSTR(IDS_DEFAULT_RESET);
                }
                node.attributes.put(new Integer(ATTRIBUTE_NAME_value),value);
            }

            if (inputtype == ATTRIBUTE_VALUE_INPUT_TEXT     ||
                inputtype == ATTRIBUTE_VALUE_INPUT_PASSWORD) {
                w = _font.stringWidth("W") * getAttributeInt(node,ATTRIBUTE_NAME_size,8) + 4;
                h = _font.getHeight() + 4;
            }
            if (inputtype == ATTRIBUTE_VALUE_INPUT_SUBMIT ||
                inputtype == ATTRIBUTE_VALUE_INPUT_RESET) {
                ASSERT(value != null);
                w = _font.stringWidth(value) + 4;
                h = _font.getHeight()        + 4;
            }
            if (inputtype == ATTRIBUTE_VALUE_INPUT_CHECKBOX ||
                inputtype == ATTRIBUTE_VALUE_INPUT_RADIO) {
                w = GUI_IMAGE_SIZE;
                h = _font.getHeight();
            }
            if (inputtype == ATTRIBUTE_VALUE_INPUT_HIDDEN) {
                w = 0;
                h = 0;
            }

            node.width  = w;
            node.height = h;

            if (_canvas.getWidth() < (FORMATING_WIDTH_LINE + w)) {
                if (!broken) {
                    breakBefore(node,0);
                }
                FORMATING_HEIGHT_LINE = h;
                FORMATING_WIDTH_LINE  = w;
            } else {
                FORMATING_WIDTH_LINE += w;
                FORMATING_HEIGHT_LINE = Math.max(FORMATING_HEIGHT_LINE,h);
            }
        }

        if (type == TAG_select) {
            node.height = heightLine * getAttributeInt(node,ATTRIBUTE_NAME_size,1) + 4;
            FORMATING_HEIGHT_LINE = Math.max(FORMATING_HEIGHT_LINE,node.height);
            node.width  = widthClient - FORMATING_WIDTH_LINE;
            if (node.width < MIN_SELECT_WIDTH) {
                FORMATING_WIDTH_LINE  = widthClient;
                if (!broken) {
                    breakBefore(node,0);
                }
                node.width = widthClient;
                FORMATING_HEIGHT_LINE = node.height;
            }
            FORMATING_WIDTH_LINE  = widthClient;
            foramtChild = false;
         }

        if (type == TAG_textarea) {
            if (!broken) {
                breakBefore(node,0);
            }
            node.height = FORMATING_HEIGHT_LINE = heightLine *
                                                  getAttributeInt(node,ATTRIBUTE_NAME_rows,1) + 4;
            node.width  = FORMATING_WIDTH_LINE  = _font.stringWidth("W") * 
                                                  getAttributeInt(node,ATTRIBUTE_NAME_cols,8) + 
                                                  8;
            foramtChild = false;
        }

        if (type == TAG_div   ||
            type == TAG_p     ||
            type == TAG_center ) {
            if (FORMATING_WIDTH_LINE != 0) {
                if (!broken) {
                    breakBefore(node,0);
                }
            }
            FORMATING_ALIGN = getAttributeInt(node,
                                              ATTRIBUTE_NAME_align,
                                              FORMATING_ALIGN);
            if (type == TAG_center || 
                (type == TAG_div && 
                    getAttributeInt(node, ATTRIBUTE_NAME_align, 
                        ATTRIBUTE_VALUE_ALIGN_LEFT) == ATTRIBUTE_VALUE_ALIGN_CENTER)) {
                FORMATING_ALIGN = ATTRIBUTE_VALUE_ALIGN_CENTER;
            }
            breakAfter  = true;
        }

        //  子ノードのレイアウト
        if (foramtChild) {
            Enumeration e = node.childNodes.elements();
            while (e.hasMoreElements()) {
                HtmlNode child = (HtmlNode)e.nextElement();
                formatNode(child);
            }
        }
        
        if (breakAfter) {
            PREV_FORMATING_ALIGN = FORMATING_ALIGN;
            DELAY_BREAK          = true;
        }

        TRACE_HTML_FORMAT("</" + DEBUGS.getTagName(type) + "> " + FORMATING_ALIGN + "->" + saved);
        FORMATING_ALIGN = saved;
        return;
    }

    /**
     *  指定したノードの直前で改行を行う.
     *
     *  @param   node   ノード
     *  @param   pos    文字列の場合どこで改行するか？
     */
    private static void breakBefore(HtmlNode node,int pos) {
        
        FORMATING_Y += FORMATING_HEIGHT_LINE;
        
        //  揃(左/中/⇒)設定に応じて、現在の行の左端位置を求める
        ASSERT(FORMATING_ALIGN == ATTRIBUTE_VALUE_ALIGN_LEFT   ||
               FORMATING_ALIGN == ATTRIBUTE_VALUE_ALIGN_RIGHT  ||
               FORMATING_ALIGN == ATTRIBUTE_VALUE_ALIGN_CENTER);

        int xBreakLine = _canvas.getWidth() - FORMATING_WIDTH_LINE;
        if (FORMATING_ALIGN == ATTRIBUTE_VALUE_ALIGN_CENTER) {
            xBreakLine = xBreakLine / 2;
        }
        if (FORMATING_ALIGN == ATTRIBUTE_VALUE_ALIGN_LEFT) {
            xBreakLine = 0;
        }
        HtmlBreakLine_xLine(FORMATING_BREAK_CURRENT) = xBreakLine;

        //  行のフォーマット計算の情報をリセットする
        FORMATING_HEIGHT_LINE = 0;
        FORMATING_WIDTH_LINE  = 0;

        //  改行位置を現在のタグの前に入れる
        HtmlBreakLine_yNext(FORMATING_BREAK_CURRENT) = FORMATING_Y;
        FORMATING_BREAK_CURRENT = newHtmlBreakLine(FORMATING_Y,pos);
        if (node.breaks == null) {
            node.breaks = new Vector();
        }
        node.breaks.addElement(FORMATING_BREAK_CURRENT);
    }

#if defined(DEBUG)
    /**
    *   ドキュメントの改行をダンプする
    */
    private static void dumpDocumentBreaks() {
        TRACE_HTML_FORMAT("> --- dump html breaks ---");
        Stack stack = new Stack();
        Enumeration e = _documentHtml.childNodes.elements();
        for(;;) {
            boolean exit = false;
            while (!e.hasMoreElements()) {
                if (stack.size() == 0) {
                    exit = true;
                    break;
                }
                e = (Enumeration)stack.pop();
            }
            if (exit) {
                break;
            }
            HtmlNode node = (HtmlNode)e.nextElement();
            if (node.breaks != null) {
                HtmlBreakLine breakLine = (HtmlBreakLine)node.breaks.elementAt(0);
                TRACE_HTML_FORMAT("<" + DEBUGS.getTagName(node.type) + "> * "
                     + "("
                     + HtmlBreakLine_xLine(breakLine)
                     + ","
                     + HtmlBreakLine_yLine(breakLine)
                     + ")"
                     );
            } else {
                TRACE_HTML_FORMAT("<" + DEBUGS.getTagName(node.type) + ">");
            }
            stack.push(e);
            e = node.childNodes.elements();
        }
        TRACE_HTML_FORMAT("< --- dump html breaks ---");
    }
#endif