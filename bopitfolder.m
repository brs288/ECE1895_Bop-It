inputFolder = uigetdir(pwd, 'Select Folder Containing BMP Images');
if inputFolder == 0
    disp('No folder selected. Exiting.');
    return;
end

bmpFiles = dir(fullfile(inputFolder, '*.bmp'));
if isempty(bmpFiles)
    error('No BMP files found in the selected folder.');
end

x1 = 144;
y1 = 49;
x2 = 176;
y2 = 67;

outputFolder = fullfile(inputFolder, 'output');
if exist(outputFolder, 'dir')
    delete(fullfile(outputFolder, '*.bmp'));
else
    mkdir(outputFolder);
end

for k = 1:length(bmpFiles)
    imgPath = fullfile(inputFolder, bmpFiles(k).name);
    img = imread(imgPath);

    if size(img, 3) ~= 3
        warning('Skipping %s: not a 24-bit BMP image.', bmpFiles(k).name);
        continue;
    end

    [height, width, ~] = size(img);
    x1c = max(1, min(x1, width));
    x2c = max(1, min(x2, width));
    y1c = max(1, min(y1, height));
    y2c = max(1, min(y2, height));

    xmin = min(x1c, x2c);
    xmax = max(x1c, x2c);
    ymin = min(y1c, y2c);
    ymax = max(y1c, y2c);

    cropped = img(ymin:ymax, xmin:xmax, :);
    outPath = fullfile(outputFolder, bmpFiles(k).name);
    imwrite(cropped, outPath, 'bmp');
end

fprintf('Saved %d cropped images to:\n%s\n', length(bmpFiles), outputFolder);
