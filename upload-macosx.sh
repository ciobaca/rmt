cd build && cmake -G Ninja -DCMAKE_BUILD_TYPE=Release .. && ninja && cd ..
zip macbinary.zip build/rmt
scp macbinary.zip stefan.ciobaca@profs.info.uaic.ro:~/public_html/rmt/
