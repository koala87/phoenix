/**
 * Kakasi API Import
 * @author Shiwei Zhang
 * @date 2014.02.10
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif
	__declspec(dllimport) int	kakasi_getopt_argv(int argc, char **argv);
	__declspec(dllimport) char*	kakasi_do(char *str);
	__declspec(dllimport) int	kakasi_close_kanwadict(void);
	__declspec(dllimport) int	kakasi_free(char *p);
#ifdef __cplusplus
}
#endif
