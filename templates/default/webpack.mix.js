let mix = require('laravel-mix');

mix.options({
    processCssUrls: false,
    terser: {
        terserOptions: {
            output: {
                comments: /@license/i
            }
        }
    }
});

mix.sass('assets/sass/style.scss', 'static/css/');

mix.scripts([
    'assets/js/licenseheader.js',
    'node_modules/@popperjs/core/dist/umd/popper.js',
    'node_modules/bootstrap/dist/js/bootstrap.js',
    'assets/js/general.js',
    'assets/js/addguest.js',
    'assets/js/addgroup.js'
], 'static/js/scripts.js');

mix.copy('node_modules/bootstrap-icons/font/fonts', 'static/fonts');
