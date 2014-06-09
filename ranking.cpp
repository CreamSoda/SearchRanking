#include "../headers/ranking.h"

double Ranking::idf(unsigned long df, unsigned long N)
{
    if(df == 0)
        return 0;
    return log10((double)N/df);
}

PostingList* Ranking::ExtendedTo200( PostingList* result ) // принимает текущее кол-во результатов
{
    for(unsigned  i = 200 - result->Length(); i > 0; i--)
        result->Add(i, 0);                                 // забить первыми документами из базы
    return result;
}
   
PostingList*Ranking::Return200(unsigned long len, PostingList* result,  vector < pair < double, unsigned long > > rank)
{
    if(len == 0 || rank.empty())             // если все найденное - стоп-слова или ни одно из слов вообще не найдено
        return ExtendedTo200(result);
        
    sort(rank.begin(), rank.end());          // сортируем полученные результаты по весу    
    for(unsigned long i = _maxRank-1, k= 0; k < 200 && k < rank.size(); k++,i--)
        result->Add(rank[i].second, 0);

    if(result->Length() < 200)                 // если меньше 200 рез-ов -- добить до 200
        return ExtendedTo200(result);
    return result; 
}

/*
double weight(unsigned long tf, unsigned long N, unsigned long df)
{
if(tf == 0)
return 0;
return (1 + log10(tf)) * idf(df, N);
}
*/

double Ranking::bm25_weight(unsigned long tf, unsigned long df, unsigned long N, int lenD, int avgDl)
{
    double k = 2.0, b = 0.75;
    return idf(df, N) * (tf * (k + 1) / (tf + k * (1 - b + b * (lenD) / avgDl ) ) );
}

pair<double, unsigned long>  Ranking::GetDocIdRank(Statistics* stat, PostingList* pl,  unsigned long docId, unsigned long df, unsigned long N, unsigned long i, double AVGlen, vector < pair < double, unsigned long > > rank )
{
    pair<double, unsigned long> tempPair;
    if(stat->GetTxtLen(docId)) // если такой документ есть 
    {
        unsigned long lenD = stat->GetTxtLen(docId);        // длина текущего документа
        unsigned long tf = pl->LengthEnt(docId);            //получить кол-во вхождений в текущий документ
        double W = bm25_weight(tf, df, N, lenD, AVGlen);	// вычислить вес слова
        
        if(i == 0)  tempPair.first = W;
        else        tempPair.first = rank[docId].first + W; // добавить вес в нужный вектор           
    }
    else
        tempPair.first = 0;

    tempPair.second = docId;
    return tempPair;
}

PostingList* Ranking::Bm25Ranking(vector<string>& query, Statistics *stat, IndexTable *idx, unsigned long N)
{
    unsigned long len = query.size();  // количество слов в запросе  
    PostingList* result = new PostingList();
    vector < pair < double, unsigned long > > rank(N);
    if(len > 0)                                             // если запрос не пуст
    {        
        double AVGlen = stat->GetAverageLen();             // средняя длина док-та
        for(unsigned long i = 0; i < len; i++)
        {
            if(!stat->IsStopWord (query[i]))                // если стоп-слово -- пропустить
            {
                PostingList* pl = idx->FindPos(query[i]);                   // загрузить постинг лист для слова из запроса
                if(pl)                                                      // если слово вообще нашлось              
                {
                    unsigned long df = pl->Length();                        // получить количество вх. слова из запроса в док-ты
                    for(unsigned long docId = 1; docId <= N; docId++)
                        rank[docId] =                                       // изменить вес текущего документа
                            GetDocIdRank(stat, pl, docId, df, N, i, AVGlen, rank);
                }
                // delete pl;
            }
        }
    }
    return Return200(len, result, rank);
}

PostingList* Ranking::VectorRancking(vector<string>& query, Statistics *stat, PostingList* Top200)
{
    PostingList* Top100 = new PostingList();


}

PostingList* Ranking::CuttingList(vector<string> &query, IndexTable *idx)
{                               
    Statistics* stat = idx->GetStat();                              // статистика
    unsigned long N = stat->GetCountTxt();                          // кол-во документов
    PostingList* Top200 = Bm25Ranking(query, stat, idx, N);
    PostingList* Top100 = VectorRancking(query, stat, Top200);
}

