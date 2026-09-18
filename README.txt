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
