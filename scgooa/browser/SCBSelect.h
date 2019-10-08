//  $Id: SCBSelect.h,v 1.1.1.1 2005/03/04 10:31:15 m_yamada Exp $

    private static void select(HtmlNode active)
    {
        TRACE_HTML_OPERATE("select:" + active);

        if (active == null) {
            TRACE("acitve node is null");
            return;
        }
        if (active.type == TAG_a) {
            String href = getAttributeStr(active,ATTRIBUTE_NAME_href);
            if (href.startsWith("mailto:")) {
//                String args = href.substring(7);    // 7 = "mailto:".length()
//                int end = args.indexOf("?");
//                String mailto;
//                String subject = null;
//                String body    = null;
//                if (end == -1) {
//                    mailto = args;
//                } else {
//                    mailto  = args.substring(0,end);
//                    args    = args.substring(end + 1);
//                    subject = getQueryStringValue(args,"subject");
//                    body    = getQueryStringValue(args,"body");
//                }
//                mailto(mailto,subject,body);
                return;

            } else  if (href.startsWith("tel:")) {
                href=href.substring(4);
                try {
                    Phone.call(href);
                } catch(Exception e) {
                    TRACE(href);
                    DEBUG_ONLY(e.printStackTrace());
                    showDialog(GETMSTR(IDS_CAN_NOT_CALL_PHONE),Dialog.DIALOG_ERROR);
                }
                return;

            } else {
                boolean utn = active.attributes.containsKey(new Integer(ATTRIBUTE_NAME_utn));
                nextDocument(href,
                             HttpConnection.GET,
                             "",
                             utn);
                return;
            }
        }

        HtmlNode form = active.parentForm;
        if (form == null) {
            //TRACE("invalid form element without no parent form. = " + DEBUGS.getTagName(active.type));
            return;
        }

        int display;
        int inputmode;
        int inputtype;

        Hashtable values = form.values;
        String name  = getAttributeStr(active,ATTRIBUTE_NAME_name);
        String value = (String)values.get(name);

        switch (active.type) {
        case TAG_select:
            TRACE_HTML_OPERATE("select:name=" + name + ",value=" + value);
            value = inputSelect(active,value);
            if (value != null) {
                TRACE_HTML_OPERATE("select(post):name=" + name + ",value=" + value);
                values.put(name,value);
            } else {
                TRACE_HTML_OPERATE("select(post):remove=" + name);
                values.remove(name);
            }
            break;

        case TAG_textarea:
            value = inputString(value,TextBox.KANA,TextBox.DISPLAY_ANY,10000);
            if (name == null) {
                return;
            }
            if (value == null) {
                value = "";
            }
            values.put(name,value);
            break;

        case TAG_input:
            inputtype = getAttributeInt(active,ATTRIBUTE_NAME_type,ATTRIBUTE_VALUE_INPUT_TEXT);
            display = TextBox.DISPLAY_ANY;

            switch(inputtype) {
            case ATTRIBUTE_VALUE_INPUT_PASSWORD:
                TRACE_HTML_OPERATE("password");
                display = TextBox.DISPLAY_PASSWORD;

            case ATTRIBUTE_VALUE_INPUT_TEXT:
                if (name == null) {
                    break;
                }
                inputmode = TextBox.KANA;

                switch(getAttributeInt(active,ATTRIBUTE_NAME_istyle,1)) {
                case 3:
                    inputmode = TextBox.ALPHA;
                    break;
                case 4:
                    inputmode = TextBox.NUMBER;
                    break;
                }
                value = inputString(value,inputmode,display,-1);
                if (value != null) {
                    values.put(name,value);
                }
                break;

            case ATTRIBUTE_VALUE_INPUT_CHECKBOX:
                TRACE_HTML_OPERATE("checkbox:name=" + name + ",value=" + value);

                if (name == null) {
                    break;
                }

                if (values.containsKey(name)) {
                    values.remove(name);
                } else {
                    value = getAttributeStr(active,ATTRIBUTE_NAME_value);
                    if (value == null) {
                        value = "";
                    }
                    values.put(name,value);
                }
                break;

            case ATTRIBUTE_VALUE_INPUT_RADIO:
                TRACE_HTML_OPERATE("radio:name=" + name + ",value=" + value);
                if (name == null) {
                    break;
                }

                values.put(name,getAttributeStr(active,ATTRIBUTE_NAME_value,""));
                break;

            case ATTRIBUTE_VALUE_INPUT_SUBMIT:
                TRACE_HTML_OPERATE("submit:name=" + name + ",value=" + value);
                if (name != null) {
                    values.put(name,form.valuesOrg.get(name));
                }
                /// @warning nameを別の用途に一時的に使っています
                name = HttpConnection.POST;
                if (getAttributeInt(form,ATTRIBUTE_NAME_method,ATTRIBUTE_VALUE_METHOD_GET) ==
                        ATTRIBUTE_VALUE_METHOD_GET) {
                    name = HttpConnection.GET;
                }
                boolean utn = form.attributes.containsKey(new Integer(ATTRIBUTE_NAME_utn));
                nextDocument(getAttributeStr(form,ATTRIBUTE_NAME_action),
                             name,
                             encodeValues(values),
                             utn);
                break;

            case ATTRIBUTE_VALUE_INPUT_RESET:
                TRACE_HTML_OPERATE("reset:name=" + name + ",value=" + value);
                values.clear();
                Enumeration e = form.valuesOrg.keys();
                while (e.hasMoreElements()) {
                    name  = (String)e.nextElement();
                    value = (String)form.valuesOrg.get(name);
                    values.put(name,value);
                }
                break;

            default:
                TRACE("*** invalid input type " + inputtype);
                break;
            }
            break;
        default:
            //TRACE("*** invalid select node " + DEBUGS.getTagName(active.type));
            break;
        }
    }

    private static String inputSelect(HtmlNode node,String selectedValue) {
        TRACE_HTML_OPERATE("inputSelect");
        Vector childs = node.childNodes;
        final int length = childs.size();
        if (length == 0) {
            TRACE_HTML_OPERATE("* select has no option.");
            return null;
        }

        String[] menus2  = new String[length];
        String[] values = new String[length];
        int selected = 0;
        int count    = 0;
        for (int index = 0;index < length;index++) {
            HtmlNode option = (HtmlNode)childs.elementAt(index);
            //TRACE("<" + DEBUGS.getTagName(option.type) + ">");
            if (option.type == TAG_option) {
                String value = getAttributeStr(option,ATTRIBUTE_NAME_value);
                if (selectedValue != null && selectedValue.equals(value)) {
                    selected = count;
                }
                values [count] = value;
                menus2 [count] = option.text;
                count++;
            }
        }
        String[] menus = new String[count];
        if (count != 0) {
            System.arraycopy(menus2,0,menus,0,count);
        }
        selected = trackMenu(null,menus,selected,true);
        if (selected == -1) {
            return selectedValue;
        }
        ASSERT(0 <= selected && selected < count);
        return values[selected];
    }
