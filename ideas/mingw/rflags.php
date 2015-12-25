<?php
return [
		'cppflags' => '-std=c++1y',
		'ldflags'  => '-mwindows',
		'compiler' => [
						'gcc'	 => 'x86_64-w64-mingw32-gcc',
						'g++'	 => 'x86_64-w64-mingw32-g++',
					],
		'suffix'   => '.exe'
		];
