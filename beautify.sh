astyle --style=kr --indent=spaces=4 --add-brackets --close-templates \
	--max-code-length=120 --indent-namespaces --pad-oper --unpad-paren \
	--align-pointer=name --align-reference=name --add-brackets \
	--break-blocks --recursive ./*.cpp ./*.h
find . -name "*.orig" -exec rm {} \;

