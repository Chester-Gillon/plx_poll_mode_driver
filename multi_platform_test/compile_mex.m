function compile_mex
    if ispc
        % Assumption is that mex is using the MingGW compiler under Windows
        mex -g multi_platform_test.c test_lib.c -I$MINGWROOT\include -liberty
        mex -g unlogger.c -R2018a
        % Convert the debug information inserted by the MingGW into the mex
        % file into PDB format to allow debugging using Visual Studio
        % elf_to_pdb_debug = fullfile(getenv('USERPROFILE'),'Downloads\cv2pdb');
        elf_to_pdb_debug = fullfile(getenv('USERPROFILE'),'cv2pdb\bin\Release\cv2pdb');
        % elf_to_pdb_debug = fullfile(getenv('USERPROFILE'),'cv2pdb-0.38\bin\Release\cv2pdb');
        system ([elf_to_pdb_debug ' -deb multi_platform_test.mexw64']);
        system ([elf_to_pdb_debug ' -deb unlogger.mexw64']);
    else
        mex -g multi_platform_test.c -lrt
        mex -g unlogger.c -R2018a
    end
end
