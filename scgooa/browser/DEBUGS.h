
#if defined(DEBUG)

private static class DEBUGS {
    #define REGIST_TAG(tokenname)     DEBUGS.registTagName(TAG_##tokenname,#tokenname)
    private static void registTagName(int code,String token) {
        s_tags.put(new Integer(code),token);
    }

    #define REGIST_ATTRIBUTE(attname) DEBUGS.registAttName(ATTRIBUTE_NAME_##attname,#attname)
    private static void registAttName(int code,String token) {
        s_attributes.put(new Integer(code),token);
    }

    private static Hashtable s_tags        = new Hashtable();
    private static Hashtable s_attributes  = new Hashtable();

    static {
        REGIST_TAG(html);
        REGIST_TAG(title);
        REGIST_TAG(body);
        REGIST_TAG(img);
        REGIST_TAG(a);
        REGIST_TAG(p);
        REGIST_TAG(div);
        REGIST_TAG(center);
        REGIST_TAG(hr);
        REGIST_TAG(br);
        REGIST_TAG(font);
        REGIST_TAG(form);
        REGIST_TAG(select);
        REGIST_TAG(option);
        REGIST_TAG(input);
        REGIST_TAG(textarea);
        REGIST_TAG(text);
        REGIST_TAG(PSEUDO_TERM);
    
        REGIST_ATTRIBUTE(bgcolor   );
        REGIST_ATTRIBUTE(text      );
        REGIST_ATTRIBUTE(link      );
        REGIST_ATTRIBUTE(alink     );
        REGIST_ATTRIBUTE(src       );
        REGIST_ATTRIBUTE(width     );
        REGIST_ATTRIBUTE(height    );
        REGIST_ATTRIBUTE(border    );
        REGIST_ATTRIBUTE(alt       );
        REGIST_ATTRIBUTE(align     );
        REGIST_ATTRIBUTE(href      );
        REGIST_ATTRIBUTE(accesskey );
        REGIST_ATTRIBUTE(color     );
        REGIST_ATTRIBUTE(size      );
        REGIST_ATTRIBUTE(direction );
        REGIST_ATTRIBUTE(behavior  );
        REGIST_ATTRIBUTE(loop      );
        REGIST_ATTRIBUTE(action    );
        REGIST_ATTRIBUTE(method    );
        REGIST_ATTRIBUTE(type      );
        REGIST_ATTRIBUTE(name      );
        REGIST_ATTRIBUTE(value     );
        REGIST_ATTRIBUTE(checked   );
        REGIST_ATTRIBUTE(maxlength );
        REGIST_ATTRIBUTE(istyle    );
        REGIST_ATTRIBUTE(rows      );
        REGIST_ATTRIBUTE(cols      );
    }

    public static String getTagName(int code) {
        return (String) s_tags.get(new Integer(code));
    }

    public static String getAttName(int code) {
        return (String) s_attributes.get(new Integer(code));
    }

    protected static String getDebugString(HtmlNode node) {
        return "<" + getTagName(node.type) + "> @" + node;
    }

    protected static void dumpHtmlNode(HtmlNode node,int depth) {
        int type    = node.type;
        String text = node.text;
        if (type == TAG_text) {
            TRACE(indent(depth) + text);

        } else {
            TRACE(indent(depth) + "<" + getTagName(type) + " " + getAttributes(node) +  ">");
        
            Enumeration e = node.childNodes.elements();
            while (e.hasMoreElements()) {
                dumpHtmlNode(((HtmlNode)e.nextElement()),depth+1);
            }
        
            TRACE(indent(depth) + "</" + DEBUGS.getTagName(type) + ">");
        }
    }

    /** 属性値をダンプする */
    protected static void dumpAttributes(HtmlNode node) {
        TRACE(getAttributes(node));
    }

    /** 属性値の一覧を取得する */
    protected static String getAttributes(HtmlNode node) {
        String dump = new String();
    
        Enumeration e = node.attributes.keys();
        while (e.hasMoreElements()) {
            Integer key   = (Integer)e.nextElement();
            Object value  = node.attributes.get(key);
        
            dump += DEBUGS.getAttName(key.intValue());
            if (value != null) {
                dump += "=" +  value;
            }
            dump += " ";
        }
    
        return dump;
    }

    /** インデント */
    private static String indent(int depth) {
        String tab = new String();
        for (int n = 0;n < depth;n++) {
            tab += "  ";
        }
        return tab;
    }

}

#endif

