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
��Ŀ->�Ҽ�����->��������->����->���Ŀ¼

Intermediate files directory:
$(SolutionDir)\Temp\$(ProjectName)\$(Configuration)
��Ŀ->�Ҽ�����->��������->����->�м�Ŀ¼

Include Path:
$(SolutionDir)\Include;$(SolutionDir)\Generic\include;
��Ŀ->�Ҽ�����->��������->VC++Ŀ¼->����Ŀ¼

Static Library Path:
$(SolutionDir)\Lib;
��Ŀ->�Ҽ�����->��������->VC++Ŀ¼->��Ŀ¼


General Setting:





