[filename, pathname] = uigetfile({'*.bmp'}, 'Select a 24-bit BMP image');
if isequal(filename, 0)
    disp('User canceled the file selection.');
    return;
end
img = imread(fullfile(pathname, filename));

if size(img, 3) ~= 3
    error('The image is not a 24-bit (true color) BMP image.');
end

x1 = 105;
y1 = 4;
x2 = 215;
y2 = 23;

x1 = max(1, min(x1, size(img, 2)));
x2 = max(1, min(x2, size(img, 2)));
y1 = max(1, min(y1, size(img, 1)));
y2 = max(1, min(y2, size(img, 1)));

xmin = min(x1, x2);
xmax = max(x1, x2);
ymin = min(y1, y2);
ymax = max(y1, y2);

cropped_img = img(ymin:ymax, xmin:xmax, :);

[savefile, savepath] = uiputfile('cropped_image.bmp', 'Save Cropped Image As');
if isequal(savefile, 0)
    disp('User canceled the save operation.');
    return;
end
imwrite(cropped_img, fullfile(savepath, savefile), 'bmp');

disp('Cropped image saved successfully.');
