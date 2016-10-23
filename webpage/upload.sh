zip rmtsrc.zip ../*.cpp ../*.h ../examples/*.rmt CMakeLists.txt
zip macbinary.zip ../main
scp rmtsrc.zip macbinary.zip index.html my.css stefan.ciobaca@profs.info.uaic.ro:~/public_html/rmt/
