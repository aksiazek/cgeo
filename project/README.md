Jednorazowa kompilacja:
        cmake CMakeLists.txt && make


Generujemy dane:
        ./datasets 10 8
        są one zapisywane w pliku lines.dat, jeżeli chcemy dodać ręcznie odcinki, można zmodyfikować plik   

Rysujemy wygenerowane odcinki
        ./paint > bare.json
        uruchamiamy gogui/gogui_visualization/index.html i wklejamy bare.json

Uruchamiamy rozwiązanie i rysujemy je:
        python range_tree.py 2.1 3.5 -1 3 | ./paint result > result.json
        uruchamiamy gogui/gogui_visualization/index.html i wklejamy result.json
      
Legenda:  
        ./datasets [ile-linii-pionowych] [ile-linii-poziomych]
        python range_tree.py [przedział x] [przedział y]
