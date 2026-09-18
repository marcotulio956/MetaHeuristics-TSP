Resultados Principais para instâncias TSPLIB 95

| **[Instância](ca://s?q=Detalhes_sobre_instancia)** | **[Tipo](ca://s?q=Detalhes_sobre_tipo)** | **n** | **Ótimo** | **Média C** | **Melhor C** | **Média SA** | **Melhor SA** | **Melh./20** | **Gap C** | **Gap SA** |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| att48 | ATT | 48 | 10628 | 10782.00 | 10782 | 10782.00 | 10782 | 0 | 1.45% | 1.45% |
| berlin52 | EUC | 52 | 7542 | 7967.00 | 7967 | 7940.10 | 7542 | 2 | 5.64% | 5.28% |
| kroA100 | EUC | 100 | 21282 | 22437.00 | 22437 | 22423.80 | 22181 | 2 | 5.43% | 5.37% |
| kroA150 | EUC | 150 | 26524 | 28516.00 | 28516 | 28464.00 | 27661 | 3 | 7.51% | 7.31% |
| kroA200 | EUC | 200 | 29368 | 30028.00 | 30028 | 30028.00 | 30028 | 0 | 2.25% | 2.25% |
| kroB100 | EUC | 100 | 22141 | 22663.00 | 22663 | 22663.00 | 22663 | 0 | 2.36% | 2.36% |
| kroB150 | EUC | 150 | 26130 | 26698.00 | 26698 | 26698.00 | 26698 | 0 | 2.17% | 2.17% |
| kroB200 | EUC | 200 | 29437 | 32489.00 | 32489 | 32339.20 | 31631 | 9 | 10.37% | 9.86% |
| kroC100 | EUC | 100 | 20749 | 22129.00 | 22129 | 22067.50 | 21357 | 4 | 6.65% | 6.35% |
| kroD100 | EUC | 100 | 21294 | 22775.00 | 22775 | 22694.85 | 22283 | 5 | 6.96% | 6.58% |
| kroE100 | EUC | 100 | 22068 | 23290.00 | 23290 | 23276.60 | 23022 | 1 | 5.54% | 5.48% |
| lin105 | EUC | 105 | 14379 | 15601.00 | 15601 | 15485.45 | 14934 | 9 | 8.50% | 7.69% |
| pr107 | EUC | 107 | 44303 | 44613.00 | 44613 | 44613.00 | 44613 | 0 | 0.70% | 0.70% |
| pr124 | EUC | 124 | 59030 | 60304.00 | 60304 | 60255.70 | 59679 | 2 | 2.16% | 2.08% |
| pr136 | EUC | 136 | 96772 | 103806.00 | 103806 | 103785.80 | 103442 | 2 | 7.27% | 7.25% |
| pr144 | EUC | 144 | 58537 | 61244.00 | 61244 | 60930.10 | 59089 | 7 | 4.62% | 4.09% |
| pr152 | EUC | 152 | 73682 | 75792.00 | 75792 | 75735.45 | 75126 | 2 | 2.86% | 2.79% |
| pr76 | EUC | 76 | 108159 | 110714.00 | 110714 | 110714.00 | 110714 | 0 | 2.36% | 2.36% |
| rat195 | EUC | 195 | 2323 | 2427.00 | 2427 | 2427.00 | 2427 | 0 | 4.48% | 4.48% |
| rat99 | EUC | 99 | 1211 | 1268.00 | 1268 | 1268.00 | 1268 | 0 | 4.71% | 4.71% |
| st70 | EUC | 70 | 675 | 691.00 | 691 | 690.85 | 688 | 1 | 2.37% | 2.35% |


Especificação

O objetivo deste trabalho é desenvolver a habilidade de implementação de heurísticas para problemas de otimização, aplicando os conceitos estudados no curso a um problema clássico de otimização: o Problema do Caixeiro Viajante (TSP, do inglês Traveling Salesman Problem). O trabalho está dividido em duas partes: (1) Implementar uma heurística construtiva, e (2) Implementar uma heurística baseada em qualquer metaheurística da literatura. A cada etapa dois arquivos devem ser submetidos no Moodle. O primeiro é um arquivo ZIP com todo o código implementado até então, e o segundo arquivo é um PDF, com uma breve descrição de cada heurística (implementada até então) e os resultados desta heurística para cada uma das instâncias de teste fornecidas no Moodle. 

As instâncias fornecidas são especificadas TSPLIB 95 se dividem em dois tipos que são aquelas onde a distância entre os pontos é euclidiana (EUC_2D) e as que a distância é pseudo euclidiana (ATT). 
Por isso, para cada tipo de instância, o cálculo da distância entre os pontos é feito de forma diferente.

1- Distância Euclidiana (EUC_2D)

Sejam x[i] e y[i] as coordenadas de um ponto i. Para as arestas do tipo EUC_2D (distância euclidiana) a distância entre dois pontos i e j é computado como segue:

xd= x[i]-x[j];
yd= y[i]-y[j];
dij= nint( sqrt( xd*xd + yd*yd));


2- Distância Euclidiana (ATT)

Sejam x[i] e y[i] as coordenadas de um ponto i. Para as arestas do tipo ATT (distância pseudo euclidiana) a distância entre dois pontos i e j é computado como segue:

xd= x[i]-x[j];
yd= y[i]-y[j];
rij= sqrt( (xd*xd + yd*yd)/10.0 );
tij= nint( rij );

if(tij<rij)
 dij= tij+1;
else
   dij= tij;


Referencias
1. Fundamentacao teorica
- Cormen et al., "Introduction to Algorithms", 2009: Capítulos 1, 2, 3 e 34 (34.1 e 34.2)
2. Heuristicas Construtivas
- Cormen et al., "Introduction to Algorithms", 2009: Capítulos 15 (15.1, 15.2, 15.3), 16 (16.1, 16.2) e 35 (35.1, 35.2, 35.3)
- Arts and Lenstra J. K.,"Local Search in Combinatorial Optimization", John Wiley and Suns, 1997: Capítulo 8 ( 8.1 e 8.2)
3. Heuristica de busca local
- E. Talbi, "Metaheuristics: From Design to Implementation", 2009: Capítulo 1
- Aarts e Lenstra, "Local Search in Combinatorial Optimization", 1997: Capítulo 8.3
4. Metaheuristicas Sequenciais
- El. Talbi, Metaheuristics: From Design to Implementation, 2009: Capítulos 2
- Sörensen, K., Sevaux, M., Glover, F. (2018). A History of Metaheuristics. In: Martí, R., Pardalos, P., Resende, M. (eds) Handbook of Heuristics. Springer, Cham. https://doi.org/10.1007/978-3-319-07124-4_4 (PDF)
 5. Metaheuristicas Populacionais
 - El. Talbi, Metaheuristics: From Design to Implementation, 2009: Capítulo 3
- Sörensen, K. (2015), Metaheuristics—the metaphor exposed. Intl. Trans. in Op. Res., 22: 3-18. https://doi.org/10.1111/itor.12001
