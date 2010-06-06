dateien=`find ./*.EPS -type f`;
for i in $dateien;
do i=${i/%".EPS"/}; convert $i.EPS $i.pdf; pdfcrop $i.pdf;
mv $i-crop.pdf $i.pdf;
done 
