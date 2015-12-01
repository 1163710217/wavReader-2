import qbs;

Project {
     references: ['3rdparty/qwt.qbs'];
     property path paLib: '/usr/local/lib/';
     property path paInclude: '/usr/local/include/';

	 CppApplication {
      Depends {name: 'Qt'; submodules: ['core','gui','widgets'] }
      Depends {name: 'PaPlayer';}
      Depends {name: 'qwt';}
      name: 'wavRead';
      files: ['*.h', '*.cpp'];

      Group {
          fileTagsFilter: ['application'];
          qbs.install: true
      }
   }
     Product {
         name: 'ByteOrder';
         files: ['lib/'+ name +'/*.h'];

         Export {
             Depends { name: 'cpp'; }
             cpp.includePaths: ['lib/'+ product.name ];
             cpp.cxxFlags: ['-std=c++0x'];
         }
     }

     StaticLibrary {
         Depends { name: 'cpp'; }
         Depends { name: 'ByteOrder'; }
         name: 'WavFileFormat';
         files: ['lib/'+ name +'/*.h', 'lib/'+ name +'/*.cpp' ];
         Export {
             Depends { name: 'cpp'; }
             Depends { name: 'ByteOrder'; }
             cpp.includePaths: ['lib/'+ product.name ];
         }
     }

     StaticLibrary {
         Depends { name: 'cpp'; }
         Depends { name: 'WavFileFormat'; }
         name: 'PaPlayer';
         files: ['lib/'+ name +'/*.h', 'lib/'+ name +'/*.cpp' ];
         cpp.includePaths: base.concat([project.paInclude]);
         Export {
             Depends { name: 'cpp'; }
             Depends { name: 'WavFileFormat'; }
             cpp.staticLibraries: ['portaudio'];
             cpp.libraryPaths: base.concat([project.paLib]);
             cpp.includePaths: base.concat(['lib/'+ product.name , project.paInclude]);
         }
     }

}
