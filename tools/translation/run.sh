sh extract_strings.sh English.utf8 directories.txt t
header_name="../../include/Nooskewl_Engine/engine_translation_English.h"
echo "std::string engine_translation_English =" > $header_name
cat English.utf8 | sed -e 's/"/\\"/g' | sed -e 's/^/"/' | sed -e 's/$/\\n"/' >> $header_name
echo ";" >> $header_name