dateien=`find ./*.EPS -type f`;
for i in $dateien;
do i=${i/%".EPS"/}; pstopdf $i.EPS $i.pdf; pdfcrop $i.pdf; rm $i.pdf;
mv $i-crop.pdf ../tmp/$i.pdf;
done 
