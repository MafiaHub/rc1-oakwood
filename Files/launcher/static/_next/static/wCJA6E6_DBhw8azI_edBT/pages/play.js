(window.webpackJsonp=window.webpackJsonp||[]).push([["1762"],{"0dXt":function(t,e,n){(window.__NEXT_P=window.__NEXT_P||[]).push(["/play",function(){var t=n("6B33");return{page:t.default||t}}])},"6B33":function(t,e,n){"use strict";n.r(e);var a=n("ln6h"),o=n.n(a),r=n("O40h"),u=n("q1tI"),c=n.n(u),i=n("b0oO"),l=n("iXVO"),d=n("y/r9"),s=function(){return window.oakwood&&oakwood.requestPlay()||console.info("mock call for requestPlay")};e.default=Object(d.a)(function(){return c.a.createElement(u.Fragment,null,c.a.createElement(i.a,null),c.a.createElement(l.b,null,"Ready to ",c.a.createElement(l.a,{onClick:s},"Play")))},Object(r.default)(o.a.mark(function t(){return o.a.wrap(function(t){for(;;)switch(t.prev=t.next){case 0:window.oakwood.checkForUpdate(),!0===window.oakwood.data.needsUpdate&&Router.push("/update");case 2:case"end":return t.stop()}},t)})))},"y/r9":function(t,e,n){"use strict";n.d(e,"a",function(){return d});var a=n("0iUn"),o=n("sLSF"),r=n("MI3g"),u=n("a7VT"),c=n("Tit0"),i=n("q1tI"),l=n.n(i),d=function(t,e){return function(n){function d(t){var e;return Object(a.default)(this,d),(e=Object(r.default)(this,Object(u.default)(d).call(this,t))).state={loading:!0,newProps:{}},e}return Object(c.default)(d,n),Object(o.default)(d,[{key:"componentDidMount",value:function(){var t=this;e().then(function(e){return t.setState({newProps:e,loading:0})})}},{key:"render",value:function(){return this.state.loading?l.a.createElement(i.Fragment,null):l.a.createElement(t,this.state.newProps)}}]),d}(i.Component)}}},[["0dXt","5d41","9da1"]]]);