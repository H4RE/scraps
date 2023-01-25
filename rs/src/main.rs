use image::{GenericImage, GenericImageView, Pixel};
fn border_reflect(p: i32, len: i32)->i32 {
   if p < 0 {
        return -p as i32;
    } else if p < len {
       return p as i32;
    } else {
       return (2 * (len - 1) - p) as i32;
    }
}

fn gaussian_filter(
    src: &image::DynamicImage,
    dest: &mut image::DynamicImage,
    radius: i8,
    sigma_x: f32,
    sigma_y: f32,
) {
    // generate gauss kernel
    let mut kernel: Vec<f32> = Vec::new();
    let mut sum: f32 = 0.0;
    for i in 0..radius{
        let v: f32 = (-(i as f32 * i as f32) / (2.0 * sigma_x * sigma_x)).exp();
        // println!("{},{},{}",i, v, sum);
        kernel.push(v);
        if i==0
        {
            sum+=v;
        }
        else {
            sum+=2.0*v;
        }
    }
    for i in 0..kernel.len()
    {
        kernel[i]/=sum;
    }

    // horizontal
    for y in 0..src.height() as i32{
        for x in 0..src.width() as i32{
            let mut pixel: image::Rgb<f32>=image::Rgb::<f32>{0:[0.0,0.0,0.0]};
            for kx in 0..kernel.len() as i32{
                let xmr = border_reflect(x-kx, src.width() as i32);
                let xpr = border_reflect(x+kx as i32, src.width() as i32);
                let pp= src.get_pixel(xpr as u32, y as u32);
                let pm= src.get_pixel(xmr as u32, y as u32);
                for c in 0..3
                {
                    let pix = (pp[c] as f32) +(pm[c] as f32);
                    pixel[c] += (pix*kernel[kx as usize]) as f32;
                }
            }
            let pix_u8 = image::Rgb::<u8>{0:[pixel[0] as u8,pixel[1] as u8,pixel[2] as u8]};

            dest.put_pixel(x as u32, y as u32, pix_u8.to_rgba());
        }
    }
    // vertical
}

fn main() {
    let src = image::open("inu.jpg").unwrap();
    let mut dest = image::DynamicImage::new_rgb8(src.width(), src.height());
    gaussian_filter(&src, &mut dest, 21, 7.0,7.0);

    dest.save("./dest.jpg");

    println!("dimensions{:?}", src.dimensions())
}
