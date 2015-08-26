#ifndef _norma_
#define _norma_
#include "nodo.h"

class Norma : public Nodo{
	private:
		int gen;

	public:
		Norma(char *);
		ListaAV * getListaAV();
		int generalizada();
		void marcarGeneralizada();

};

#endif
