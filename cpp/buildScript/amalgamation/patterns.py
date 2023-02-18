import re


class Patterns:
    raw_twoslash_comment_pattern = re.compile(r"//.*?\n")
    raw_threeslash_comment_pattern = re.compile(r"///.*?($|\n)")
    raw_quote_comment_pattern = re.compile(r"/\*.*?\*/", re.S)
    include_pattern = re.compile(
        r'(^|\n)#\s*?include\s*?(<|")(?P<path>.*?)("|>)',
        re.S
    )
    string_pattern = re.compile("[^']" r'".*?(?<=[^\\])"', re.S)
