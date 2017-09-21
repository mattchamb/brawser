var assert = require('assert');

var asdf = require('../brawser.js')

describe('Array', function() {
  describe('#indexOf()', function() {
    it('should return -1 when the value is not present', function() {
        var asd = new asdf.LibRaw();
        var mem = asdf._malloc(2);

        var imageData = new Uint8Array([10, 9]);
        
        asdf.HEAPU8.set(imageData, mem);
        console.log(mem);
        asdf._free(mem);
        asd.delete();
    });
  });
});