(window.webpackJsonp=window.webpackJsonp||[]).push([["1762"],{"0dXt":function(e,t,n){(window.__NEXT_P=window.__NEXT_P||[]).push(["/play",function(){var e=n("6B33");return{page:e.default||e}}])},"6B33":function(e,t,n){"use strict";n.r(t);var o=n("ln6h"),a=n.n(o),r=n("O40h"),c=n("q1tI"),u=n.n(c),i=n("b0oO"),l=n("iXVO"),d=n("y/r9"),s=function(){return window.oakwood&&oakwood.requestPlay()||console.info("mock call for requestPlay")};t.default=Object(d.a)(function(){return u.a.createElement(c.Fragment,null,u.a.createElement(i.a,null),u.a.createElement(l.b,null,"Ready to ",u.a.createElement(l.a,{onClick:s},"Play")))},Object(r.default)(a.a.mark(function e(){return a.a.wrap(function(e){for(;;)switch(e.prev=e.next){case 0:try{window.external.invoke("init")}catch(t){}if(window.oakwood){e.next=3;break}return e.abrupt("return",console.info("mock call for checkForUpdate"));case 3:console.log(window.oakwood.checkForUpdates),window.oakwood.checkForUpdates(),!0===window.oakwood.data.needsUpdate&&Router.push("/update");case 6:case"end":return e.stop()}},e)})))},"y/r9":function(e,t,n){"use strict";n.d(t,"a",function(){return d});var o=n("0iUn"),a=n("sLSF"),r=n("MI3g"),c=n("a7VT"),u=n("Tit0"),i=n("q1tI"),l=n.n(i),d=function(e,t){return function(n){function d(e){var t;return Object(o.default)(this,d),(t=Object(r.default)(this,Object(c.default)(d).call(this,e))).state={loading:!0,newProps:{}},t}return Object(u.default)(d,n),Object(a.default)(d,[{key:"componentDidMount",value:function(){var e=this;t().then(function(t){return e.setState({newProps:t,loading:0})})}},{key:"render",value:function(){return this.state.loading?l.a.createElement(i.Fragment,null):l.a.createElement(e,this.state.newProps)}}]),d}(i.Component)}}},[["0dXt","5d41","9da1"]]]);