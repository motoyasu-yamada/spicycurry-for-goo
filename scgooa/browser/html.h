/*-----------------------------------------------------------------------------
	サーバーで圧縮されたHTMLトークンの値
	
	$Id: html.h,v 1.1.1.1 2005/03/04 10:31:15 m_yamada Exp $
	$Revision: 1.1.1.1 $
	$Date: 2005/03/04 10:31:15 $
	$Author: m_yamada $

-------------------------------------------------------------------------------*/

// HTMLタグ種別
#define TAGTYPE_BEG_ELEMENT 0
#define TAGTYPE_END_ELEMENT 1
#define TAGTYPE_EMP_ELEMENT 2
#define TYPE_EOF     0xC0
#define TYPE_ELEMENT 0xC1

/**
*   タグ-トークンID
*/  
#define TAG_title       1
#define TAG_body        2
#define TAG_img         3
#define TAG_a           4
#define TAG_p           5
#define TAG_div         6
#define TAG_center      7
#define TAG_hr          8
#define TAG_br          9
#define TAG_font       10
#define TAG_form       11
#define TAG_select     12
#define TAG_option     13
#define TAG_input      14
#define TAG_textarea   15

#define TAG_html       1000
#define TAG_text       1001
//  本来は存在しない擬似タグ - ドキュメント終端タグ
#define TAG_PSEUDO_TERM 1002

// 属性-トークンID
#define ATTRIBUTE_NAME_bgcolor   33
#define ATTRIBUTE_NAME_text      34
#define ATTRIBUTE_NAME_link      35
#define ATTRIBUTE_NAME_alink     36
#define ATTRIBUTE_NAME_src       37
#define ATTRIBUTE_NAME_width     38
#define ATTRIBUTE_NAME_height    39
#define ATTRIBUTE_NAME_border    40
#define ATTRIBUTE_NAME_alt       41
#define ATTRIBUTE_NAME_align     42
#define ATTRIBUTE_NAME_href      43
#define ATTRIBUTE_NAME_accesskey 44
#define ATTRIBUTE_NAME_color     45
#define ATTRIBUTE_NAME_size      46
#define ATTRIBUTE_NAME_direction 47
#define ATTRIBUTE_NAME_behavior  48
#define ATTRIBUTE_NAME_loop      49
#define ATTRIBUTE_NAME_action    50
#define ATTRIBUTE_NAME_method    51
#define ATTRIBUTE_NAME_type      52
#define ATTRIBUTE_NAME_name      53
#define ATTRIBUTE_NAME_value     54
#define ATTRIBUTE_NAME_checked   55
#define ATTRIBUTE_NAME_maxlength 56
#define ATTRIBUTE_NAME_istyle    57
#define ATTRIBUTE_NAME_rows      58
#define ATTRIBUTE_NAME_cols      59
#define ATTRIBUTE_NAME_selected  60
#define ATTRIBUTE_NAME_utn       61

// 属性値の型
#define ATTRIBUTE_VTYPE_STR    0
#define ATTRIBUTE_VTYPE_FLAG   1
#define ATTRIBUTE_VTYPE_INT8   2
#define ATTRIBUTE_VTYPE_INT24  3
#define ATTRIBUTE_VTYPE_PINT   4
#define ATTRIBUTE_VTYPE_TOKEN  5

#define ATTRIBUTE_VALUE_INVALID         -1
#define ATTRIBUTE_VALUE_ALIGN_LEFT      0
#define ATTRIBUTE_VALUE_ALIGN_CENTER    1
#define ATTRIBUTE_VALUE_ALIGN_RIGHT     2
#define ATTRIBUTE_VALUE_METHOD_GET      3
#define ATTRIBUTE_VALUE_METHOD_POST     4
#define ATTRIBUTE_VALUE_INPUT_TEXT      5
#define ATTRIBUTE_VALUE_INPUT_PASSWORD  6
#define ATTRIBUTE_VALUE_INPUT_CHECKBOX  7
#define ATTRIBUTE_VALUE_INPUT_RADIO     8
#define ATTRIBUTE_VALUE_INPUT_HIDDEN    9
#define ATTRIBUTE_VALUE_INPUT_SUBMIT    10
#define ATTRIBUTE_VALUE_INPUT_RESET     11
