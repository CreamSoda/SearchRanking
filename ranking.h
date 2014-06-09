#ifndef RANKING_H
#define RANKING_H

#include "definesincludes.h"
#include "posting.h"
#include "index.h"
#include "dbworker.h"
#include <algorithm>
class Ranking{
	unsigned long _maxRank;
	DBWorker _db;

public:
	Ranking(unsigned long maxRank = 200):_maxRank(maxRank){}
    PostingList* CuttingList(vector<string>&, IndexTable*);
    PostingList* Bm25Ranking(vector<string>&, Statistics *, IndexTable *, unsigned long);  // ранжирование bm25
	PostingList* tf_idfRanking();                       // обычное tf-idf ранжирование
	PostingList* VectorRancking(vector<string>&, Statistics*, PostingList*); //векторное ранжирование
	PostingList* ModelRancking();                       // ранжирование с использованием языковых моделей 
    PostingList* ExtendedTo200(Statistics*, PostingList*);          // дополнить кол-во результатов поиска до 200
    PostingList* Return200(unsigned long, Statistics*, PostingList*, vector <pair <double, unsigned long> >);//вернуть результат чернового ранжирования
    pair<double, unsigned long>  GetDocIdRank(Statistics*, PostingList*,  unsigned long, unsigned long, unsigned long, unsigned long, double, vector < pair < double, unsigned long > >); // возвращает вес док-та для текущего запроса
    double bm25_weight(unsigned long, unsigned long, unsigned long, int, int);
    double idf(unsigned long, unsigned long);
};

#endif // RANKING_H
