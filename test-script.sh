folder="sample-text"
files=$(ls $folder)

for file in $files;
do
    ./compress $folder/$file > /dev/null
    ./uncompress $folder/$file > /dev/null
    if !(diff $folder/$file $folder/$file.uncompress > err.txt) ; 
    then
        echo $file and $file.uncompress do not match
    else
        echo $file and $file.uncompress match
    fi
done

rm $folder/*.compress.codes
rm $folder/*.compress
rm $folder/*.uncompress


