cats=$(cat ../../nom_WS/cats.cfg | grep -v "#" | grep ":" | cut -d ":" -f 1)
d_tilde=$(ls ../yield_sys/csv/ | grep b2 | grep -v SM | cut -d '_' -f 4)
bin=$(ls ../yield_sys/csv/ | grep m01 | cut -d '_' -f 5 | cut -d '.' -f 1)

echo $d_tilde
echo $bin
echo $cats
