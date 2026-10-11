from pathlib import Path
from PIL import Image, ImageDraw, ImageFont
root=Path(__file__).resolve().parents[1]
out=root/'build/fangzheng-phoenix-preview.png'
fonts=[('Current pixel 12', 'C:/ONS_FPGA/方正像素12.ttf',12,False),
 ('Phoenix pixel 12 / mono','C:/Users/lin13/Downloads/凤凰点阵体 12px_爱给网_aigei_com.ttf',12,False)]
canvas=Image.new('RGB',(600,110*len(fonts)),'white');d=ImageDraw.Draw(canvas)
for i,(label,path,size,gray) in enumerate(fonts):
 f=ImageFont.truetype(path,size)
 print(label,f.getname(),f.getbbox('备份存档'),f.getlength('备'))
 d.text((10,i*110+5),label,fill='black')
 mask=Image.new('L' if gray else '1',(180,size+2))
 for j,c in enumerate('备份存档恢复时钟读取设置'):
  ImageDraw.Draw(mask).text((j*size,0),c,font=f,fill=255 if gray else 1)
 sample=Image.new('RGB',mask.size,'white');sample.paste('#172f49',(0,0),mask)
 canvas.paste(sample,(10,i*110+22))
 canvas.paste(sample.resize((540,(size+2)*3),Image.Resampling.NEAREST),(10,i*110+45))
canvas.save(out)
print(out)
