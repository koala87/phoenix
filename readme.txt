Directory Hierarchy:

KTVServer -- source files

Generic -- generic API functions
    io/net/sql/time/utility...

Include -- the third party header file

Lib -- the third party static library

Temp -- compile/link temporary files
    Temp/$(ProjectName)/$(Configuration) 
Bin -- store exe/lib/dll
    Bin/$(ProjectName)/$(Configuration)



Visual Studio Setting:

Output directory:
$(SolutionDir)\Bin\$(ProjectName)\$(Configuration)
项目->右键属性->配置属性->常规->输出目录

Intermediate files directory:
$(SolutionDir)\Temp\$(ProjectName)\$(Configuration)
项目->右键属性->配置属性->常规->中间目录

Include Path:
$(SolutionDir)\Include;$(SolutionDir)\Generic\include;
项目->右键属性->配置属性->VC++目录->包含目录

Static Library Path:
$(SolutionDir)\Lib;
项目->右键属性->配置属性->VC++目录->库目录


General Setting:





