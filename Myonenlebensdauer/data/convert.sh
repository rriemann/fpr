dateien=`find ./*.EPS -type f`;
for i in $dateien;
do i=${i/%".EPS"/}; ps2pdf $i.EPS ../tmp/$i.pdf;
done 
