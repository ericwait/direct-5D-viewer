function writeHeaderArray(fid,name,val)
    tst = num2str(val(:),'%.9g');
    fieldstr = ['%' num2str(size(tst,2)) '.9g'];

    fprintf(fid,'float %s[] = \n{\n',name);
    for i=1:size(val,2)
        for j=1:size(val,1)
            fprintf(fid,fieldstr,val(j,i));
            if ( i < size(val,2) || j < size(val,1) )
                fprintf(fid,',');
            end
        end
    %     if ( i < size(val,2) )
            fprintf(fid,'\n');
    %     end
    end
    fprintf(fid,'};\n');
end
