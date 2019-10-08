    /** 描画用 - グローバル - テキスト色 */
    private static int DOCUMENT_COLOR_TEXT;
    /** 描画用 - グローバル - 背景色 */
    private static int DOCUMENT_COLOR_BG;
    /** 描画用 - グローバル - リンク色 */
    private static int DOCUMENT_COLOR_LINK;
    /**
     *  描画用:表示されるドキュメント領域の上端Y座標
     */
    private static int _drawingVisibleTop;
    /**
     *  描画用:表示されるドキュメント領域の下端Y座標
     */
    private static int _drawingVisibleBottom;
    /**
     *  描画用:アンカー内かどうか？
     */
    private static boolean _drawingInAnchor;
    /**
     *  描画用:アクティブオブジェクト内かどうか?
     */
    private static boolean _drawingInActive;
    /**
     *  描画用:水平表示位置
     */
    private static int _drawingX;
    /**
     *  描画用:垂直表示位置
     */
    private static int _drawingY;
    /**
     *  描画用:現在描画中の行の下端のY座標
     */
    private static int _drawingYNext;
    /**
     *  フォントタグのネスティングの深さ
     */
    private static int _drawingFontNestDepth;

    /**
     *  表示
     *  @param g 描画対象グラフィックコンテキスト
     */
    private final static boolean drawNode(Graphics g,HtmlNode node) {
        ASSERT(node != null);
        ASSERT(g != null);

        g.setPictoColorEnabled(true);
        boolean drawContinue = true;
        final boolean anchorSaved = _drawingInAnchor;
        final boolean activeSaved = _drawingInActive;
        final int colorSaved      = DOCUMENT_COLOR_TEXT;
        final int fontSaved       = _drawingFontNestDepth;
        final int type = node.type;
        if (type == TAG_text) {
            if (_drawingFontNestDepth == 0) {
                g.setPictoColorEnabled(false);
            }
            g.setColor(DOCUMENT_COLOR_TEXT);
            if (_drawingInAnchor) {
                g.setColor(DOCUMENT_COLOR_LINK);
            }
            String str = node.text;
            final int length = str.length();
            if (length == 0) {
                if (node.breaks != null) {
                    HtmlBreakLine breakLine = (HtmlBreakLine)node.breaks.elementAt(0);
                    _drawingY = HtmlBreakLine_yLine(breakLine);
                    _drawingYNext = HtmlBreakLine_yNext(breakLine);
                    _drawingX = HtmlBreakLine_xLine(breakLine);
                }
                return true;
            }
            
            int start = 0;
            int breakIndex = 0;
            int breakCount = 0;
            if (node.breaks != null) {
                breakCount = node.breaks.size();
            }
            do {
                if (_drawingVisibleBottom <= _drawingY) {
                    return false;
                }
                // 描画範囲を決定
                HtmlBreakLine breakLine = null;
                int posBreak  = length;
                if (breakIndex < breakCount) {
                    breakLine = (HtmlBreakLine)node.breaks.elementAt(breakIndex);
                    posBreak  = HtmlBreakLine_pos(breakLine);
                }
                breakIndex++;

                if (_drawingVisibleTop <= _drawingYNext) {
                    // 描画対象文字を抽出
                    String line = str.substring(start,posBreak);
                    int lineWidth = _font.stringWidth(line);
                    start = posBreak;

                    // アンカーがアクティブな場合
                    if (_drawingInAnchor) {
                        g.setColor(DOCUMENT_COLOR_LINK);
                    } else {
                        g.setColor(DOCUMENT_COLOR_TEXT);
                    }
                    if (_drawingInActive) {
                        // g.setColor(DOCUMENT_COLOR_LINK);
                        g.fillRect(_drawingX,_drawingY,lineWidth,_font.getHeight());
                        g.setColor(DOCUMENT_COLOR_BG);
                    }
                
                    // 文字描画
                    g.drawString(line,
                                 _drawingX,
                                 _drawingY + _font.getAscent());

                    // アンカーアンダーラインを描画
                    if (_drawingInAnchor) {
                        g.drawLine(_drawingX,
                                   _drawingY + _font.getHeight()  -1,
                                   _drawingX + lineWidth,
                                   _drawingY + _font.getHeight() -1);
                    }
                    
                     _drawingX += lineWidth;
                }

                // 改行処理
                if (breakLine != null) {
                    _drawingY = HtmlBreakLine_yLine(breakLine);
                    _drawingYNext = HtmlBreakLine_yNext(breakLine);
                    _drawingX = HtmlBreakLine_xLine(breakLine);
                }
            } while (breakIndex <= breakCount);

            return true;
        }

        //
        //  タグの描画
        //
        if (node.breaks != null) {
            HtmlBreakLine breakLine = (HtmlBreakLine)node.breaks.elementAt(0);
            _drawingY = HtmlBreakLine_yLine(breakLine);
            _drawingX = HtmlBreakLine_xLine(breakLine);
            _drawingYNext = HtmlBreakLine_yNext(breakLine);
        }

        boolean drawChild = true;
        if (_drawingVisibleTop <= _drawingYNext) {
            if (type == TAG_title) {
                return true;
            }
            if (type ==  TAG_img) {
                int src = getAttributeInt(node,ATTRIBUTE_NAME_src,-1);
                MediaImage image;
                if (0 <= src && src < _inlineImages.size()) {
                    image = (MediaImage)_inlineImages.elementAt(src);
                    if (image != null) {
                        g.drawImage(image.getImage(),_drawingX,_drawingY);
                    }
                    if (_drawingInActive) {
                        g.setColor(DOCUMENT_COLOR_LINK);
                        g.drawRect(_drawingX  ,_drawingY  ,node.width-1,node.height-1);
                        g.drawRect(_drawingX+1,_drawingY+1,node.width-2,node.height-2);
                    }
                }
                _drawingX += node.width;
            }
            if (type == TAG_hr) {
                int size    = Math.min(getAttributeInt(node,ATTRIBUTE_NAME_size,1),2);
                int hrwidth = getAttributePixelOrPercent(node,ATTRIBUTE_NAME_width,100/*=100%*/);
                g.setColor(getAttributeColor(node,ATTRIBUTE_NAME_color,Graphics.GRAY));
                g.fillRect((_canvas.getWidth() - hrwidth) / 2,
                           _drawingY + (_font.getHeight() - size)/ 2,
                           hrwidth,
                           size);
            }
            if (type == TAG_a) {
                if (node.isInteractive) {
                    _drawingInAnchor = true;
                    _drawingInActive = INTERACTIVES.elementAt(ACTIVE_INTERACTIVE_INDEX) == node;
                }
            }
            if (type ==  TAG_font) {
                DOCUMENT_COLOR_TEXT = getAttributeColor(node,
                                                        ATTRIBUTE_NAME_color,
                                                        DOCUMENT_COLOR_TEXT);
                _drawingFontNestDepth++;
            }
            if (node.parentForm != null &&
                (type == TAG_textarea ||
                 type == TAG_input    ||
                 type == TAG_select)) {
                Hashtable values = node.parentForm.values;
                int w = node.width;
                int h = node.height;
                boolean isActive = INTERACTIVES.elementAt(ACTIVE_INTERACTIVE_INDEX) == node;

                String name  = getAttributeStr(node,ATTRIBUTE_NAME_name);
                // String value = null; nameが空の場合 vaueもnullになってしまう
                String value = getAttributeStr(node,ATTRIBUTE_NAME_value);
                // name属性がhtmlで指定されていない場合
                // ダミーのname属性が指定されている。
                if (!name.startsWith(">")) {
                    value = (String)values.get(name);
                }

                int temp  = _colors[isActive ? COLOR_SELECT_BKGND : COLOR_CLIENT_BKGND];
                int temp2 = _colors[isActive ? COLOR_SELECT_TEXT  : COLOR_CLIENT_TEXT];
                g.setColor(DOCUMENT_COLOR_TEXT);
                if (type == TAG_select) {
                    if (value != null) {
                        String data    = null;
                        Enumeration options = node.childNodes.elements();
                        while (options.hasMoreElements()) {
                            HtmlNode option = (HtmlNode)options.nextElement();
                            String value1 = getAttributeStr(option,ATTRIBUTE_NAME_value);
                            if (value1 != null && value1.equals(value)) {
                                data = option.text;
                                break;
                            }
                        }
                        value = data;
                    }

                    drawBox(g,_drawingX,_drawingY,w - GUI_IMAGE_SIZE,h,temp,_colors[COLOR_CLIENT_FRAME]);
                    setColorOfRGB(g,temp2);
                    drawText(g,value,_drawingX,_drawingY,w - GUI_IMAGE_SIZE,h,2,true,ALIGN_LEFT);

                    int imageIndex = isActive ? GUI_IMAGE_INDEX_SELECT_FOCUS : GUI_IMAGE_INDEX_SELECT;
                    g.drawImage(imgGui,
                                   _drawingX + w - GUI_IMAGE_SIZE,
                                   _drawingY,
                                   GUI_IMAGE_SIZE * imageIndex,
                                   0,
                                   GUI_IMAGE_SIZE,
                                   GUI_IMAGE_SIZE);
                }
                if (type == TAG_textarea) {
                    drawBox(g,_drawingX,_drawingY,w,h,temp,_colors[COLOR_CLIENT_FRAME]);
                    setColorOfRGB(g,temp2);
                    drawText(g,value,_drawingX,_drawingY,w,h,2,true,ALIGN_LEFT);
                }
                if (type == TAG_input) {
                    int inputtype = getAttributeInt(node,ATTRIBUTE_NAME_type,ATTRIBUTE_VALUE_INPUT_TEXT);
                    if (inputtype == ATTRIBUTE_VALUE_INPUT_TEXT ||
                        inputtype == ATTRIBUTE_VALUE_INPUT_PASSWORD) {
                        drawBox(g,_drawingX,_drawingY,w,h,temp,_colors[COLOR_CLIENT_FRAME]);
                        setColorOfRGB(g,temp2);
                        drawText(g,value,_drawingX,_drawingY,w,h,2,true,ALIGN_LEFT);
                    }
                    if (inputtype == ATTRIBUTE_VALUE_INPUT_RADIO) {
                        TRACE_HTML_DRAW("radio");
                        int imageIndex = GUI_IMAGE_INDEX_RADIO;
                        if (value != null &&
                            value.equals(getAttributeStr(node,ATTRIBUTE_NAME_value))) {
                            imageIndex += GUI_IMAGE_OFFSET_CHECKED;
                        }
                        if (isActive) {
                            imageIndex += GUI_IMAGE_OFFSET_FOCUS;
                        }
                        g.drawImage(imgGui,
                                       _drawingX,
                                       _drawingY,
                                       GUI_IMAGE_SIZE * imageIndex,
                                       0,
                                       GUI_IMAGE_SIZE,
                                       GUI_IMAGE_SIZE);
                    }
                    if (inputtype == ATTRIBUTE_VALUE_INPUT_CHECKBOX) {
                        int imageIndex = GUI_IMAGE_INDEX_CHECKBOX;
                        if (value != null) {
                            imageIndex += GUI_IMAGE_OFFSET_CHECKED;
                        }
                        if (isActive) {
                            imageIndex += GUI_IMAGE_OFFSET_FOCUS;
                        }
                        g.drawImage(imgGui,
                                       _drawingX,
                                       _drawingY,
                                       GUI_IMAGE_SIZE * imageIndex,
                                       0,
                                       GUI_IMAGE_SIZE,
                                       GUI_IMAGE_SIZE);
                    }

                    if (inputtype == ATTRIBUTE_VALUE_INPUT_SUBMIT ||
                        inputtype == ATTRIBUTE_VALUE_INPUT_RESET) {
                        temp  = _colors[isActive ? COLOR_ACTIVE_BUTTON_BKGND : COLOR_BUTTON_BKGND];
                        temp2 = _colors[isActive ? COLOR_ACTIVE_BUTTON_TEXT  : COLOR_BUTTON_TEXT];
                        value = getAttributeStr(node,ATTRIBUTE_NAME_value);
                        drawBox(g,_drawingX,_drawingY,w,h,temp,_colors[COLOR_BUTTON_FRAME]);
                        setColorOfRGB(g,temp2);
                        drawText(g,value,_drawingX,_drawingY,w,h,2,false,ALIGN_LEFT);
                    }
                }

                _drawingX += w;
                drawChild = false;
            }
        }

        if (_drawingVisibleBottom <= _drawingY) {
            drawContinue = false;
        } else {
            if (drawChild) {
                final int childs = node.childNodes.size();
                for (int n = 0;n < childs;n++) {
                    if (!drawNode(g,(HtmlNode)node.childNodes.elementAt(n)))  {
                        drawContinue = false;
                        break;
                    }
                }
            }
        }

        _drawingInAnchor   = anchorSaved;
        _drawingInActive   = activeSaved;
        DOCUMENT_COLOR_TEXT = colorSaved;
        _drawingFontNestDepth  = fontSaved;
        return drawContinue;
    }
