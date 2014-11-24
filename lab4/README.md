Usage:

        1) cmake CMakeLists.txt && make
        
        2) ./polygon [dataset] which emits json file, 
        e.g. ./polygon test.dat > polygon.json
        
        3) use gogui/gogui_visualization/index.html to paste json file and see triangulation animation
        
        4) create own y-monotone polygons using canvas.html:
           Caution! Please draw clockwise. The last line is automatically added when processing, no need to perfectly hit the first point :)
           Additionally, the point buffer is not automatically reset when the page is reloaded, do this manually.
        
        
