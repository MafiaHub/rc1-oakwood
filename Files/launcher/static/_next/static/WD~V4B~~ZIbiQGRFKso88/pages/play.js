(window.webpackJsonp=window.webpackJsonp||[]).push([["1762"],{"0dXt":function(t,e,n){(window.__NEXT_P=window.__NEXT_P||[]).push(["/play",function(){var t=n("6B33");return{page:t.default||t}}])},"6B33":function(t,e,n){"use strict";n.r(e);var a=n("ln6h"),o=n.n(a),r=n("O40h"),c=n("q1tI"),u=n.n(c),i=n("b0oO"),l=n("iXVO"),d=n("y/r9"),s=function(){return window.oakwood&&oakwood.requestPlay()||console.info("mock call for requestPlay")};e.default=Object(d.a)(function(){return u.a.createElement(c.Fragment,null,u.a.createElement(i.a,null),u.a.createElement(l.b,null,"Ready to ",u.a.createElement(l.a,{onClick:s},"Play")))},Object(r.default)(o.a.mark(function t(){return o.a.wrap(function(t){for(;;)switch(t.prev=t.next){case 0:try{window.external.invoke("init")}catch(e){}if(window.oakwood){t.next=3;break}return t.abrupt("return",console.info("mock call for checkForUpdate"));case 3:window.oakwood.checkForUpdates().catch(function(){console.info("wtf")}),!0===window.oakwood.data.needsUpdate&&Router.push("/update");case 5:case"end":return t.stop()}},t)})))},"y/r9":function(t,e,n){"use strict";n.d(e,"a",function(){return d});var a=n("0iUn"),o=n("sLSF"),r=n("MI3g"),c=n("a7VT"),u=n("Tit0"),i=n("q1tI"),l=n.n(i),d=function(t,e){return function(n){function d(t){var e;return Object(a.default)(this,d),(e=Object(r.default)(this,Object(c.default)(d).call(this,t))).state={loading:!0,newProps:{}},e}return Object(u.default)(d,n),Object(o.default)(d,[{key:"componentDidMount",value:function(){var t=this;e().then(function(e){return t.setState({newProps:e,loading:0})})}},{key:"render",value:function(){return this.state.loading?l.a.createElement(i.Fragment,null):l.a.createElement(t,this.state.newProps)}}]),d}(i.Component)}}},[["0dXt","5d41","9da1"]]]);