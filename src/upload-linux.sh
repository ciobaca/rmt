cd build && cmake -G Ninja -DCMAKE_BUILD_TYPE=Release .. && ninja && cd ..
zip linuxbinary.zip build/rmt
scp linuxbinary.zip stefan.ciobaca@profs.info.uaic.ro:~/public_html/rmt/
