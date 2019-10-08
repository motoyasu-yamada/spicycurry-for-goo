    /**
     *  インライン画像キャッシュ
     */
    private static Vector _inlineImages = new Vector();

    /**
    *   HTMLドキュメントを解析する
    */
    protected static void parseDocument(byte[] buffer)  {
        ASSERT(buffer != null);
        if (buffer.length == 0) {
            return;
        }

        INTERACTIVES = new Vector();
        ACCESSKEYS   = new HtmlNode[12];
        ACTIVE_INTERACTIVE_INDEX = 0;

        ByteArrayInputStream input = new ByteArrayInputStream(buffer);
        HtmlNode form = null;
        Vector stackNodes = new Vector();

        HtmlNode current = _documentHtml = new HtmlNode();
        current.type = TAG_html;
        stackNodes.addElement(current);

        CURRENT_URL = readString(input);
        for (;;) {
            int b = input.read();

            final int type = (b >> 6) & 3;
            final int tag  = b & 0x3F;

            if (type == TAGTYPE_BEG_ELEMENT ||
                type == TAGTYPE_EMP_ELEMENT) {

                TRACE_HTML_LOAD("<" + DEBUGS.getTagName(tag) + ">(" + tag + ")");

                HtmlNode newNode = new HtmlNode();
                newNode.type = tag;
                ASSERT(current != null);
                ASSERT(current.childNodes != null);
                current.childNodes.addElement(newNode);
                stackNodes.addElement(current);
                current = newNode;
                loadAttributes(input,current);
                if (tag == TAG_a) {
                    if (getAttributeStr(current,ATTRIBUTE_NAME_href) != null) {
                        setInteractivity(current);
                    }
                }
                if (tag == TAG_form) {
                    form = current;
                    current.values    = new Hashtable();
                    current.valuesOrg = new Hashtable();
                    autocomplete(CURRENT_URL,current.values);
                    autocomplete(CURRENT_URL,current.valuesOrg);
                }
                if (tag == TAG_input) {
                    if (getAttributeInt(current,ATTRIBUTE_NAME_type,ATTRIBUTE_VALUE_INPUT_TEXT)
                            != ATTRIBUTE_VALUE_INPUT_HIDDEN) {
                        setInteractivity(current);
                    }
                    setFormNode(form,current);
                }
                if (tag == TAG_select ||
                    tag == TAG_textarea) {
                    setInteractivity(current);
                }
                //  空要素タグの場合は開いた同時に閉じる
                //  開タグの場合は次に遷移する
                if (type == TAGTYPE_BEG_ELEMENT) {
                    continue;
                }
            }

            //  終端タグ又は空要素タグ
            if (type == TAGTYPE_END_ELEMENT ||
                type == TAGTYPE_EMP_ELEMENT ) {
                TRACE_HTML_LOAD("</" + DEBUGS.getTagName(tag) + ">");
                if (tag == TAG_title) {
                    childText2Attribute(current);
                    _documentTitle = current.text;
                }
                if (tag ==  TAG_textarea) {
                    childText2Attribute(current);
                    setFormNode(form,current);
                }
                if (tag ==  TAG_select) {
                    setFormNode(form,current);
                }
                if (tag ==  TAG_option) {
                    TRACE("OPTION=");
                    childText2Attribute(current);
                }
                if (tag == TAG_form) {
                    autocomplete(CURRENT_URL,current.values);
                    autocomplete(CURRENT_URL,current.valuesOrg);
                }
                ASSERT(!stackNodes.isEmpty());
                current = (HtmlNode)stackNodes.lastElement();
                stackNodes.removeElementAt(stackNodes.size()-1);
                continue;
            }
            //  文字要素
            if (b == TYPE_ELEMENT) {
                String element = readString(input);
                HtmlNode node = new HtmlNode();
                node.type = TAG_text;
                node.text = element;
                ASSERT(current != null);
                ASSERT(current.childNodes != null);
                current.childNodes.addElement(node);
                continue;
            }

            ASSERT(b == TYPE_EOF);
            break;
        } // for-loop

        //  ドキュメント末尾に</div>がある場合に
        //  正しくレイアウトするために擬似終端タグを入れる
        HtmlNode term = new HtmlNode();
        term.type     = TAG_PSEUDO_TERM;
        _documentHtml.childNodes.addElement(term);

        ASSERT(_documentHtml != null);
        if (_documentTitle == null) {
            _documentTitle = GETMSTR(IDS_UNNAMED_DOCUMENT_TITLE);
        }

        //  インライン画像の展開
        int images = input.read() & 0xFF;
        TRACE("loadInlineImage:" + images);

        for (int n = 0;n < images;n++) {
            int length = ((input.read() & 0xFF) << 8) + (input.read() & 0xFF);
            TRACE("loadInlineImage:" + n + ":" + length);
            MediaImage image = null;
            if (length != 0) {
                byte[] tmp = new byte[length];
                input.read(tmp,0,length);
                image = loadMediaI(tmp);
            }
            _inlineImages.addElement(image);
        }
        System.gc();
    }

    /**
        option/textarea等の子要素のテキスト値を、
        要素のtextフィールドに格納する。
    */
    private static void childText2Attribute(HtmlNode current) {
        Vector childs = current.childNodes;
        current.text  = "";
        if (childs.size() != 0) {
            HtmlNode child = (HtmlNode)childs.elementAt(0);
           // TRACE("<" + DEBUGS.getTagName(child.type) + ">(" + child + ")" + child.text);
            ASSERT(child.type == TAG_text);
            current.text += child.text;
        }
        childs.removeAllElements();
    }


    /**
    *   フォームノードを設定する
    */
    private static void setFormNode(HtmlNode form,HtmlNode node) {
        if (form == null) {
            TRACE_HTML_LOAD("invalid child without the parent form.");
            return;
        }

        node.parentForm = form;

        String name  = getAttributeStr(node,ATTRIBUTE_NAME_name);
        if (name == null) {
            name = ">" + node.toString();
            node.attributes.put(new Integer(ATTRIBUTE_NAME_name),name);
        }

        String value = null;
        boolean add = true;
        switch(node.type) {
        case TAG_input:
            value = getAttributeStr(node,ATTRIBUTE_NAME_value);
            switch(getAttributeInt(node,ATTRIBUTE_NAME_type,ATTRIBUTE_VALUE_INPUT_TEXT)) {
            case ATTRIBUTE_VALUE_INPUT_CHECKBOX:
                if (!node.attributes.containsKey(new Integer(ATTRIBUTE_NAME_checked))) {
                    value = null;
                } else {
                    if (value == null) {
                        value = "";
                    }
                }
                break;
            case ATTRIBUTE_VALUE_INPUT_RADIO:
                if (!node.attributes.containsKey(new Integer(ATTRIBUTE_NAME_checked))) {
                    value = null;
                }
                break;
            case ATTRIBUTE_VALUE_INPUT_RESET:
            case ATTRIBUTE_VALUE_INPUT_SUBMIT:
                add = false;
                break;
            }
            break;
        case TAG_textarea:
            value = node.text;
            break;
        case TAG_select:
            Enumeration options = node.childNodes.elements();
            while (options.hasMoreElements()) {
                HtmlNode option = (HtmlNode)options.nextElement();
                if (option.attributes.containsKey(new Integer(ATTRIBUTE_NAME_selected))) {
                    value = getAttributeStr(option,ATTRIBUTE_NAME_value);
                    TRACE("selected:" + value);
                    break;
                }
            }
            break;
#if defined(DEBUG)

        default:
            TRACE("invalid type." + DEBUGS.getDebugString(node));
            DEBUGS.dumpAttributes(node);
#endif

        }
        if (value == null) {
            return;
        }
        if (add) {
            form.values.put(name,value);
        }
        form.valuesOrg.put(name,value);
    }

    private static void loadAttributes(ByteArrayInputStream input,HtmlNode current) {
        for (;;) {
            int name = input.read();
            if (name == 0) {
                return;
            }

            TRACE_HTML_LOAD(DEBUGS.getAttName(name));

            int head = input.read();
            int type = head >> 5;
            Object value = null;

            switch(type) {
            case ATTRIBUTE_VTYPE_STR:
                TRACE_HTML_LOAD("ATTRIBUTE_VTYPE_STR");
                value = readString(input);
                break;

            case ATTRIBUTE_VTYPE_FLAG:
                TRACE_HTML_LOAD("ATTRIBUTE_VTYPE_FLAG");
                value = "";
                break;

            case ATTRIBUTE_VTYPE_INT8:
            case ATTRIBUTE_VTYPE_PINT:
                TRACE_HTML_LOAD("ATTRIBUTE_VTYPE_INT8/PINT " + type);
                value = new Integer(input.read());
                break;

            case ATTRIBUTE_VTYPE_INT24:
                TRACE_HTML_LOAD("ATTRIBUTE_VTYPE_INT24");
                value = new Integer((input.read() << 16) +
                                    (input.read() <<  8) + input.read());
                break;

            case ATTRIBUTE_VTYPE_TOKEN:
                TRACE_HTML_LOAD("ATTRIBUTE_VTYPE_TOKEN");
                value = new Integer(head & 31);
                break;

#if defined(DEBUG)

            default:
                TRACE("#" + __LINE__+": head=" + head);
                return;
#endif

            }

            TRACE_HTML_LOAD(value);
            current.attributes.put(new Integer(name),value);
        }
    }

    /**
        インタラクティブなノードを、
        インタラクティブ・ノード配列と
        アクセスキー・ノード配列に格納する
    */
    private static void setInteractivity(HtmlNode node) {
        node.isInteractive = true;
        INTERACTIVES.addElement(node);
        int accesskey;
        if ((accesskey =  getAttributeInt(node,ATTRIBUTE_NAME_accesskey,-1)) == -1) {
            return;
        }
        ACCESSKEYS[accesskey-1] = node;
    }

    /**
     *  画像キャッシュの破棄
     *  HTMLドキュメント内の画像の実体を全て破棄(unuse/dispose)する。
     *  @see _inlineImages
     */
    private static void disposeImageCache() {
        int count = _inlineImages.size();
        for (int n = 0;n < count;n++) {
            MediaImage mi = (MediaImage)_inlineImages.elementAt(n);
            if (mi != null) {
                mi.unuse();
                mi.dispose();
            }
        }
        _inlineImages.removeAllElements();
        System.gc();
    }
