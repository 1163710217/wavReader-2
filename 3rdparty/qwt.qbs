import qbs;

CppApplication {
            Depends { name: "Qt"; submodules: ["core", "gui","widgets","concurrent","printsupport"] }            
            name: "qwt"
				  	type:'staticlibrary';
            cpp.defines: base.concat(['QWT_NO_SVG','QWT_NO_OPENGL'])
            property path  location_lib: 'qwt-6.1/'
					  files: [location_lib +'*.h', location_lib+'*.cpp']
            excludeFiles: [location_lib +'qwt_plot_svgitem.*', location_lib +'qwt_plot_glcanvas.*']
            Export {
                Depends { name :"cpp" }
                Depends { name: "Qt"; submodules: ["core", "gui","widgets","concurrent","printsupport"] }
                cpp.includePaths: [  product.location_lib ]
            }

        }
